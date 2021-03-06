#include "gtest\gtest.h"
#include <algorithm>

#include "Board.h"

TEST(BoardTest, testConstructor)
{
	Board b;
	EXPECT_EQ(0, b.GetNumTiles());
}

TEST(BoardTest, testMakeBoardNumTiles)
{
	Board b;
	b.MakeBoard(1);
	EXPECT_EQ(7, b.GetNumTiles());

	for (int i = 2; i < 4; ++i)
	{
		Board b;
		b.MakeBoard(i);
		EXPECT_EQ(19, b.GetNumTiles());
	}

	for (int i = 4; i < 8; ++i)
	{
		Board b;
		b.MakeBoard(i);
		EXPECT_EQ(37, b.GetNumTiles());
	}

	for (int i = 8; i < 13; ++i)
	{
		Board b;
		b.MakeBoard(i);
		EXPECT_EQ(61, b.GetNumTiles());
	}
}

void 
testMakeBoardTileTypes(const int& numPlayers)
{
	Board b;
	b.MakeBoard(numPlayers);

	std::unordered_map<ResourceType, int> tileTypesCount;
	for (int i = 0; i < b.GetNumTiles(); ++i)
	{
		const auto type = b.GetTileType(i);
		auto iter = tileTypesCount.find(type);
		if (iter == tileTypesCount.end())
			tileTypesCount[type] = 1;
		else
			iter->second++;
	}
	for (auto type : tileTypesCount)
	{
		if (type.first != ResourceType::WATER)
			EXPECT_EQ(numPlayers, type.second);
	}
}

TEST(BoardTest, testMakeBoardTileTypes)
{
	for (int i = 2; i <= 10; ++i)
	{
		testMakeBoardTileTypes(i);
	}
}

void
testMakeBoardHarvestRates(const int& numPlayers)
{
	Board b;
	b.MakeBoard(numPlayers);
	for (int i = 0; i < b.GetNumTiles(); ++i)
	{
		if (b.GetTileType(i) != ResourceType::WATER)
			EXPECT_EQ(1, b.GetHarvestRate(i));
	}
}

TEST(BoardTest, testMakeBoardHarvestRates)
{
	for (int i = 2; i <= 10; ++i)
	{
		testMakeBoardHarvestRates(i);
	}
}

void
testSetHarvestRates(const Board& b, int expectedRate)
{
	for (int tile = 0; tile < b.GetNumTiles(); ++tile)
	{
		if (b.GetTileType(tile) == ResourceType::WATER)
			EXPECT_EQ(0, b.GetHarvestRate(tile));
		else
			EXPECT_EQ(expectedRate, b.GetHarvestRate(tile));
	}
}

TEST(BoardTest, testSetHarvestRates)
{
	for (int i = 2; i <= 10; ++i)
	{
		Board b;
		b.MakeBoard(i);
		
		for (int rate = -1; rate < 5; ++rate)
		{
			for (int tile = 0; tile < b.GetNumTiles(); ++tile)
			{
				b.SetHarvestRate(tile, rate);
			}

			int expectedRate = std::max(rate, 1);
			testSetHarvestRates(b, expectedRate);
		}
	}
}

void
testIsPositionValid(const int& numPlayers)
{
	Board b;
	b.MakeBoard(numPlayers);
	const int numTiles = b.GetNumTiles();
	for (int i = 0; i < numTiles; ++i)
	{
		EXPECT_TRUE(b.IsPositionValid(b.GetTileCoord(i)));
	}

	AxialCoord pos = b.GetTileCoord(0);
	const int boardRadius = std::max(std::abs(pos.r), std::abs(pos.q));
	const int outOfBounds = boardRadius + 1;
	for (int q = -outOfBounds; q <= outOfBounds; ++q)
	{
		for (int r = -outOfBounds; r <= outOfBounds; ++r)
		{
			AxialCoord curPos = { r, q };
			if (abs(curPos.q) > boardRadius ||
				abs(curPos.r) > boardRadius ||
				abs(curPos.r + curPos.q) > boardRadius)
			{
				EXPECT_FALSE(b.IsPositionValid(curPos));
			}
			else
			{
				EXPECT_TRUE(b.IsPositionValid(curPos));
			}
		}
	}
}

TEST(BoardTest, testIsPositionValid)
{
	for (int i = 2; i <= 10; ++i)
	{
		testIsPositionValid(i);
	}
}


int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}