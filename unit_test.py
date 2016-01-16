from random import randint

import sys
sys.path.append(sys.argv[1])
from indexer import n_gramm

storage = {}

alphabet = [chr(i) for i in xrange(65, 123)]
alphabetSize = len(alphabet)

GOD_WORLD_COUNT = 10
BAD_WORLD_COUNT = 10


def generateWorld(len):
	return ''.join(alphabet[randint(0, alphabetSize-1)] for _ in xrange(len))

def createGoodWorld(pattern1, pattern2, pattern3):
	return generateWorld(randint(3,10)) + pattern1 + generateWorld(randint(3,10)) + \
	pattern2 + generateWorld(randint(3,10)) + pattern3 + generateWorld(randint(3,10))

def createBadWorld(pattern1, pattern2, pattern3):
	return generateWorld(randint(3,10)) + pattern2 + generateWorld(randint(3,10)) + \
	pattern1 + generateWorld(randint(3,10)) + pattern1 + generateWorld(randint(3,10))

def test(n):
	storage = {}
	index = n_gramm(n)
	pattern1 = generateWorld(randint(3,10))
	pattern2 = generateWorld(randint(3,10))
	pattern3 = generateWorld(randint(3,10))
	patternJoin = '%s*%s*%s' % (pattern1, pattern2, pattern3)

	newIdx = []
	for i in xrange(GOD_WORLD_COUNT + BAD_WORLD_COUNT):
		idx = n*1000 + i
		newIdx.append(idx)
		storage[idx] = s = createGoodWorld(pattern1, pattern2, pattern3) \
			if i < GOD_WORLD_COUNT else createBadWorld(pattern1, pattern2, pattern3)
		index.add_line(idx,s)
	assert (index.size() == GOD_WORLD_COUNT + BAD_WORLD_COUNT)
	for _ in xrange(1):
		assert (len(index.search('*')) == GOD_WORLD_COUNT + BAD_WORLD_COUNT)
		assert (len(index.search(pattern3)) == GOD_WORLD_COUNT)
		assert (len(index.search(pattern3+'!')) == 0)
		assert (len(index.search(pattern1)) == GOD_WORLD_COUNT + BAD_WORLD_COUNT)
		assert (len(index.search(patternJoin)) == GOD_WORLD_COUNT)

	for idx in newIdx:
		index.del_line(idx)
	assert (index.size() == 0)

if __name__ == "__main__":
	for _ in xrange(1000):
		for n in xrange(3,13):
			test(n)