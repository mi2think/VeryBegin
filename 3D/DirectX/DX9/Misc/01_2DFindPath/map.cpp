#include "map.h"

Map::Map()
	: rng_(2016925)
	, waitInput_(WI_ToPos)
{
}

void Map::Init()
{
	// init back lines
	InitBackLines();

	// init grids
	InitGrids();

	// init blocks
	InitBlocks();

	// generate from position
	while (true)
	{
		uint32 index = rng_.RandomUInt32(0, grids_.size());
		if (!grids_[index].isBlock)
		{
			// auto input from position first
			Input(grids_[index].pt);
			break;
		}
	}

	findpath2D_.SetPathJudge(this);
}

void Map::Render(const Timestep& timestep)
{

}

void Map::Input(const Vector2i& position)
{
	if (!IsReachable(position))
		return;

	if (waitInput_ = WI_None)
	{
		// input from position
		fromPos_ = Vector2i(position.x / GRID_WIDTH, position.y / GRID_HEIGHT);
		fromRect_.SetRect(fromPos_.x * GRID_WIDTH, fromPos_.y * GRID_HEIGHT, (fromPos_.x + 1) * GRID_WIDTH, (fromPos_.y + 1) * GRID_HEIGHT);
		waitInput_ = WI_ToPos;
	}
	else if (waitInput_ = WI_ToPos)
	{
		// input to position
		toPos_ = Vector2i(position.x / GRID_WIDTH, position.y / GRID_HEIGHT);
		toRect_.SetRect(toPos_.x * GRID_WIDTH, toPos_.y * GRID_HEIGHT, (toPos_.x + 1) * GRID_WIDTH, (toPos_.y + 1) * GRID_HEIGHT);
		waitInput_ = WI_None;
	}

	if (waitInput_ == WI_None)
	{
		if (findpath2D_.FindPath(fromPos_, toPos_))
		{
			paths_.clear();
			const auto& vec = findpath2D_.GetPath();
			paths_.reserve(vec.size());
			for (const auto& e : vec )
			{
				paths_.push_back(GetGridCenterPt(e.y, e.x));
			}
		}
	}
}

void Map::InitBackLines()
{
	backLines_.reserve((GRID_X_NUM + 1) * 2 + (GRID_Y_NUM + 1) * 2);

	// x parallel lines
	for (uint32 i = 0; i < GRID_Y_NUM + 1; ++i)
	{
		Vector2f pt;
		pt.x = 0;
		pt.y = i * GRID_HEIGHT;
		backLines_.push_back(pt);

		pt.x = GRID_X_NUM * GRID_WIDTH;
		pt.y = i * GRID_HEIGHT;
		backLines_.push_back(pt);
	}
	// y parallel lines
	for (uint32 i = 0; i < GRID_X_NUM + 1; ++i)
	{
		Vector2f pt;
		pt.x = i * GRID_WIDTH;
		pt.y = 0;
		backLines_.push_back(pt);

		pt.x = i * GRID_WIDTH;
		pt.y = GRID_Y_NUM * GRID_HEIGHT;
		backLines_.push_back(pt);
	}
}

void Map::InitGrids()
{
	grids_.reserve(GRID_X_NUM * GRID_Y_NUM);

	for (uint32 j = 0; j < GRID_Y_NUM; ++j)
	{
		for (uint32 i = 0; i < GRID_X_NUM; ++i)
		{
			Grid grid;
			grid.pt = Vector2i(i, j);
			grid.isBlock = false;
			grids_.push_back(grid);
		}
	}
}

void Map::InitBlocks()
{
	blocks_.resize(BLOCK_NUM);

	// generate BLOCK_NUM random number:[1, GRID_X_NUM * GRID_Y_NUM]
	// output as position: row = num / GRID_X_NUM, col = num % GRID_X_NUM
	RNG rng(2016925);
	uint32* buffer = new uint32[BLOCK_NUM];
	rng.RandomUInt32(buffer, BLOCK_NUM, 1, GRID_X_NUM * GRID_Y_NUM);
	for (uint32 i = 0; i < BLOCK_NUM; ++i)
	{
		uint32 idx = buffer[i];
		uint32 row = idx / GRID_X_NUM;
		uint32 col = idx % GRID_X_NUM;
		grids_[idx].isBlock = true;

		Rectf rect;
		rect.SetRect(col * GRID_WIDTH, row * GRID_HEIGHT, (col + 1) * GRID_WIDTH, (row + 1) * GRID_HEIGHT);
		blocks_[i] = rect;
	}
	delete[] buffer;
}