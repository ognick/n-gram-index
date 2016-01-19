from random import randint

import sys
if len(sys.argv) > 1:
	sys.path.append(sys.argv[1])
from indexer import n_gramm

alphabet1 = [chr(i) for i in xrange(65, 123)]
alphabet2 = [chr(i) for i in xrange(123, 188)]

GOD_WORLD_COUNT = 100
BAD_WORLD_COUNT = 100

def RAND():
	return randint(5, 100)

def generateWorld(alphabet, length):
	return ''.join(alphabet[randint(0, len(alphabet)-1)] for _ in xrange(length))

def createGoodWorld(pattern1, pattern2):
	return generateWorld(alphabet1, RAND()) + pattern1 + generateWorld(alphabet1, RAND()) + pattern2 + generateWorld(alphabet1, RAND())

def createBadWorld(pattern3):
	return generateWorld(alphabet2, RAND()) + pattern3 + generateWorld(alphabet2, RAND())

def test(n):
	storage = {}
	index = n_gramm(n)
	pattern1 = generateWorld(alphabet1, RAND()) + '1'
	pattern2 = generateWorld(alphabet1, RAND()) + '2'
	pattern3 = generateWorld(alphabet2, RAND()) + '3'
	patternRight = '%s*%s' % (pattern1, pattern2)
	patternWrong = '%s*%s' % (pattern2, pattern1)

	for i in xrange(GOD_WORLD_COUNT + BAD_WORLD_COUNT):
		idx = n*1000 + i
		storage[idx] = s = createGoodWorld(pattern1, pattern2) \
			if i < GOD_WORLD_COUNT else createBadWorld(pattern3)
		index.add_line(idx,s)
	assert (index.size() == GOD_WORLD_COUNT + BAD_WORLD_COUNT)
	for _ in xrange(1):
		assert (len(index.search('*')) == GOD_WORLD_COUNT + BAD_WORLD_COUNT)
		assert (len(index.search(pattern3)) == BAD_WORLD_COUNT)
		assert (len(index.search(pattern1)) == GOD_WORLD_COUNT)
		assert (len(index.search(patternRight)) == GOD_WORLD_COUNT)
		assert (len(index.search(patternWrong)) == 0)

	for idx in storage.iterkeys():
		index.del_line(idx)
	assert (index.size() == 0)

if __name__ == "__main__":
	for _ in xrange(1):
		for n in xrange(3, 13):
			test(n)