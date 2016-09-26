#pragma once
#include "sims.h"
#include "math/vector2.h"
#include "math/rectangle.h"
#include "math/rng.h"
#include "core/find_path2d.h"
using namespace sims;

#define GRID_X_NUM	120
#define GRID_Y_NUM	60

#define GRID_WIDTH	10
#define GRID_HEIGHT	10

#define BLOCK_NUM	1500

class Map : public IPathJudge
{
public:
	struct Grid 
	{
		Vector2i pt;
		bool isBlock;
	};

	enum WaitInput
	{
		WI_None,
		WI_FromPos,
		WI_ToPos,
	};

	Map();

	void Init();
	void Render(const Timestep& timestep);
	void Input(const Vector2i& position);

	const Grid& GetGrid(uint32 row, uint32 col) const
	{ 
		return grids_[row * GRID_X_NUM + col]; 
	}

	Vector2f GetGridCenterPt(uint32 row, uint32 col)
	{
		Vector2f pt;
		pt.x = col * GRID_WIDTH + GRID_WIDTH * 0.5f;
		pt.y = row * GRID_HEIGHT + GRID_HEIGHT * 0.5f;
		return pt;
	}

	virtual bool IsReachable(const Vector2i& pt)
	{
		if (pt.x < 0 || pt.x > GRID_X_NUM || pt.y < 0 || pt.y >= GRID_Y_NUM)
			return false;
		return ! GetGrid(pt.x, pt.y).isBlock;
	}
private:
	void InitBackLines();
	void InitGrids();
	void InitBlocks();
private:
	vector<Vector2f>  backLines_;
	vector<Grid> grids_;
	vector<Rectf> blocks_;
	vector<Vector2f> paths_;

	Vector2i fromPos_;
	Vector2i toPos_;
	Rectf fromRect_;
	Rectf toRect_;

	WaitInput waitInput_;

	RNG rng_;
	FindPath2D findpath2D_;
};
