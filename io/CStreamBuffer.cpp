#include "CStreamBuffer.h"

//
// CStreamBuffer
//

const UInt32			CStreamBuffer::kChunkSize = 4096;

CStreamBuffer::CStreamBuffer() :
	m_size(0),
	m_headUsed(0)
{
	// do nothing
}

CStreamBuffer::~CStreamBuffer()
{
	// do nothing
}

const void*
CStreamBuffer::peek(UInt32 n)
{
	assert(n <= m_size);

	// reserve space in first chunk
	ChunkList::iterator head = m_chunks.begin();
	head->reserve(n + m_headUsed);

	// consolidate chunks into the first chunk until it has n bytes
	ChunkList::iterator scan = head;
	++scan;
	while (head->size() - m_headUsed < n && scan != m_chunks.end()) {
		head->insert(head->end(), scan->begin(), scan->end());
		scan = m_chunks.erase(scan);
	}

	return reinterpret_cast<const void*>(&(head->begin()[m_headUsed]));
}

void
CStreamBuffer::pop(UInt32 n)
{
	// discard all chunks if n is greater than or equal to m_size
	if (n >= m_size) {
		m_size     = 0;
		m_headUsed = 0;
		m_chunks.clear();
		return;
	}

	// update size
	m_size -= n;

	// discard chunks until more than n bytes would've been discarded
	ChunkList::iterator scan = m_chunks.begin();
	assert(scan != m_chunks.end());
	while (scan->size() - m_headUsed <= n) {
		n         -= scan->size() - m_headUsed;
		m_headUsed = 0;
		scan       = m_chunks.erase(scan);
		assert(scan != m_chunks.end());
	}

	// remove left over bytes from the head chunk
	if (n > 0) {
		m_headUsed += n;
	}
}

void
CStreamBuffer::write(const void* vdata, UInt32 n)
{
	assert(vdata != NULL);

	// ignore if no data, otherwise update size
	if (n == 0) {
		return;
	}
	m_size += n;

	// cast data to bytes
	const UInt8* data = reinterpret_cast<const UInt8*>(vdata);

	// point to last chunk if it has space, otherwise append an empty chunk
	ChunkList::iterator scan = m_chunks.end();
	if (scan != m_chunks.begin()) {
		--scan;
		if (scan->size() >= kChunkSize) {
			++scan;
		}
	}
	if (scan == m_chunks.end()) {
		scan = m_chunks.insert(scan, Chunk());
	}

	// append data in chunks
	while (n > 0) {
		// choose number of bytes for next chunk
		assert(scan->size() <= kChunkSize);
		UInt32 count = kChunkSize - scan->size();
		if (count > n)
			count = n;

		// transfer data
		scan->insert(scan->end(), data, data + count);
		n    -= count;
		data += count;

		// append another empty chunk if we're not done yet
		if (n > 0) {
			++scan;
			scan = m_chunks.insert(scan, Chunk());
		}
	}
}

UInt32
CStreamBuffer::getSize() const
{
	return m_size;
}
