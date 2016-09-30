#include "terrain.h"
#include "core/color.h"
#include "core/file_system.h"
#include "core/file_input_stream.h"
#include "platform/platform.h"
#include "utils/geometry_gen.h"

Terrain::Terrain(IDirect3DDevice9* device, 
	const string& heightMapFilename, 
	float heightScale, 
	float width, 
	float depth, 
	uint32 slices, 
	uint32 stacks)
	: device_(device)
	, width_(width)
	, depth_(depth)
	, slices_(slices)
	, stacks_(stacks)
	, numVertices_(slices * stacks)
	, numIndices_((stacks - 1) * (slices - 1) * 6)
	, numTriangles_(numIndices_ / 3)
	, heightScale_(heightScale)
	, vb_(nullptr)
	, ib_(nullptr)
	, tex_(nullptr)
	, vertices_(nullptr)
	, indices_(nullptr)
{
	bool b = ReadRawFile(heightMapFilename);
	ASSERT(b && "read height map file failed!");

	// scale height
	for (auto& e : heightmap_)
	{
		e *= heightScale_;
	}

	GenerateVertices();
	GenerateIndices();
}

Terrain::~Terrain()
{
	SAFE_RELEASE(vb_);
	SAFE_RELEASE(ib_);
	SAFE_RELEASE(tex_);

	SAFE_DELETEARRAY(vertices_);
	SAFE_DELETEARRAY(indices_);
}

float Terrain::GetHeightmapEntry(uint32 row, uint32 col) const
{
	return heightmap_[row * slices_ + col];
}

void Terrain::SetHeightmapEntry(uint32 row, uint32 col, float value)
{
	heightmap_[row * slices_ + col] = value;
}

bool Terrain::ReadRawFile(const string& heightMapFilename)
{
	// RAW file dimensions must be >=0
	auto fs = Platform::GetFileSystem();
	auto stream = fs->OpenInputStream(heightMapFilename);
	ASSERT(stream);

	vector<uint8> buffer = stream->Read();
	heightmap_.resize(buffer.size());
	for (uint32 i = 0; i < buffer.size(); ++i)
	{
		heightmap_[i] = buffer[i];
	}
	return true;
}

void Terrain::GenerateVertices()
{
	vertices_ = new dx9::VertexUV[numVertices_];

	float xStep = width_ / (slices_ - 1);
	float zStep = depth_ / (stacks_ - 1);
	float xOrigin = -width_ / 2;
	float zOrigin = -depth_ / 2;

	float uOrigin = 0.0f;
	float vOrigin = 1.0f;
	float uStep = 1.0f / (slices_ - 1);
	float vStep = -1.0f / (stacks_ - 1);

	for (uint32 j = 0; j < stacks_; ++j) // z
	{
		for (uint32 i = 0; i < slices_; ++i) // x
		{
			uint32 index = j * slices_ + i;
			uint8* v = (uint8*)(vertices_ + index);
			Vector3f* p = (Vector3f*)(v + offsetof(dx9::VertexUV, p));
			p->x = xOrigin + i * xStep;
			p->z = zOrigin + j * zStep;
			// since we generate vertices from left bottom
			// but height map pixel is from left top
			// we need map correct height map index
			p->y = GetHeightmapEntry(stacks_ - 1 - j, i);

			Vector2f* uv = (Vector2f*)(v + offsetof(dx9::VertexUV, uv));
			uv->x = uOrigin + i * uStep;
			uv->y = vOrigin + j * vStep;
		}
	}

	dx9::CHECK_HR = device_->CreateVertexBuffer(numVertices_ * sizeof(dx9::VertexUV),
		D3DUSAGE_WRITEONLY,
		dx9::VertexUV::FVF,
		D3DPOOL_MANAGED,
		&vb_,
		0);
	dx9::VertexUV* v = nullptr;
	dx9::CHECK_HR = vb_->Lock(0, 0, (void**)&v, 0);
	memcpy(v, vertices_, numVertices_ * sizeof(dx9::VertexUV));
	dx9::CHECK_HR = vb_->Unlock();
}

void Terrain::GenerateIndices()
{
	uint16* indices_ = new uint16[numIndices_];

	uint16 triStride = 3 * sizeof(uint16);
	for (uint32 j = 0; j < stacks_ - 1; ++j)
	{
		for (uint32 i = 0; i < slices_ - 1; ++i)
		{
			uint32 triIndex = 2 * ((slices_ - 1) * j + i);

			uint32 offset = triIndex * triStride;
			GeometryGen::TriIndex16* p = (GeometryGen::TriIndex16*)((uint8*)indices_ + offset);
			p->i0 = (uint16)(j * slices_ + i);
			p->i1 = (uint16)((j + 1) * slices_ + i);
			p->i2 = (uint16)((j + 1) * slices_ + i + 1);

			offset += triStride;
			p = (GeometryGen::TriIndex16*)((uint8*)indices_ + offset);
			p->i0 = (uint16)(j * slices_ + i);
			p->i1 = (uint16)((j + 1) * slices_ + i + 1);
			p->i2 = (uint16)(j * slices_ + i + 1);
		}
	}

	dx9::CHECK_HR = device_->CreateIndexBuffer(numIndices_ * sizeof(uint16),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&ib_,
		0);
	WORD* i = nullptr;
	dx9::CHECK_HR = ib_->Lock(0, 0, (void**)&i, 0);
	memcpy(i, indices_, numIndices_ * sizeof(uint16));
	dx9::CHECK_HR = ib_->Unlock();
}

bool Terrain::LoadTexture(const string& filename)
{
	dx9::CHECK_HR = D3DXCreateTextureFromFile(device_, filename.c_str(), &tex_);
	return tex_ != nullptr;
}

bool Terrain::GenerateTexture(const Vector3f& dirLight)
{
	// method fills top surface of a texture procedurally.
	// then lights the top surface.
	// Finally it fill other mipmap surfaces based on top surface data using D3DXFilterTexture

	// texel for each quad cell
	uint32 texWidth  = slices_ - 1;
	uint32 texHeight = stacks_ - 1;

	// create an empty texture
	dx9::CHECK_HR = D3DXCreateTexture(device_,
		texWidth,
		texHeight,
		0, // create a complete mipmap chain
		0,
		D3DFMT_X8R8G8B8,
		D3DPOOL_MANAGED,
		&tex_);
	D3DSURFACE_DESC desc;
	dx9::CHECK_HR = tex_->GetLevelDesc(0, &desc);
	if (desc.Format != D3DFMT_X8R8G8B8)
		return false;

	// diffuse color
	D3DLOCKED_RECT lockedRect;
	dx9::CHECK_HR = tex_->LockRect(0, &lockedRect, 0, 0);
	uint8* imageData = (uint8*)lockedRect.pBits;
	for (uint32 i = 0; i < texHeight; ++i)
	{
		uint32* rowData = (uint32*)imageData;
		for (uint32 j = 0; j < texWidth; ++j)
		{
			// get height of upper left vertex of quad
			float height = GetHeightmapEntry(i, j) / heightScale_;
			
			Color c;
			if (height < 42.5f)
				c = Color(255, 249, 157);
			else if (height < 85.0f)
				c = Color(124, 197, 118);
			else if (height < 127.5f)
				c = Color(0, 166, 81);
			else if (height < 170.0f)
				c = Color(25, 123, 48);
			else if (height < 212.5f)
				c = Color(115, 100, 87);
			else
				c = Color(255, 255, 255);

			rowData[j] = c.value;
		}
		imageData += lockedRect.Pitch;
	}
	dx9::CHECK_HR = tex_->UnlockRect(0);
	D3DXSaveTextureToFile("gen_tex.png", D3DXIFF_PNG, tex_, 0);

	if (!LightTerrain(dirLight))
		return false;

	dx9::CHECK_HR = D3DXFilterTexture(tex_, 0, 0, D3DX_DEFAULT);
	D3DXSaveTextureToFile("gen_ligting_tex.png", D3DXIFF_PNG, tex_, 0);

	return true;
}

void Terrain::Draw(const D3DXMATRIX& world, bool drawWireframe)
{
	if (device_)
	{
		dx9::CHECK_HR = device_->SetTransform(D3DTS_WORLD, &world);
		dx9::CHECK_HR = device_->SetStreamSource(0, vb_, 0, sizeof(dx9::VertexUV));
		dx9::CHECK_HR = device_->SetFVF(dx9::VertexUV::FVF);
		dx9::CHECK_HR = device_->SetIndices(ib_);
		dx9::CHECK_HR = device_->SetTexture(0, tex_);

		// turn off lighting since we are lighting it self
		dx9::CHECK_HR = device_->SetRenderState(D3DRS_LIGHTING, false);

		if (!drawWireframe)
		{

			dx9::CHECK_HR = device_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
				0,
				0,
				numVertices_,
				0,
				numTriangles_);
		}
		else
		{
			dx9::CHECK_HR = device_->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			dx9::CHECK_HR = device_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
				0,
				0,
				numVertices_,
				0,
				numTriangles_);
			dx9::CHECK_HR = device_->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}


		dx9::CHECK_HR = device_->SetRenderState(D3DRS_LIGHTING, true);
	}
}

bool Terrain::LightTerrain(const Vector3f& dirLight)
{
	D3DSURFACE_DESC desc;
	dx9::CHECK_HR = tex_->GetLevelDesc(0, &desc);
	if (desc.Format != D3DFMT_X8R8G8B8)
		return false;

	// modulate diffuse color with light color
	D3DLOCKED_RECT lockedRect;
	dx9::CHECK_HR = tex_->LockRect(0, &lockedRect, 0, 0);
	uint8* imageData = (uint8*)lockedRect.pBits;
	for (uint32 i = 0; i < desc.Height; ++i)
	{
		uint32* rowData = (uint32*)imageData;
		for (uint32 j = 0; j < desc.Width; ++j)
		{
			// current color of quad
			Color c(rowData[j]);
			// shade current quad
			c *= ComputeShade(i, j, dirLight);
			rowData[j] = c.value;

		}
		imageData += lockedRect.Pitch;
	}
	dx9::CHECK_HR = tex_->UnlockRect(0);
	return true;
}

float Terrain::ComputeShade(uint32 cellRow, uint32 cellCol, const Vector3f& dirLight)
{
	// get height of three vertices on the quad
	//  A---B
	//  + 
	//  C
	float heightA = GetHeightmapEntry(cellRow, cellCol);
	float heightB = GetHeightmapEntry(cellRow, cellCol + 1);
	float heightC = GetHeightmapEntry(cellRow + 1, cellCol);

	// build two vectors on the quad
	Vector3f u(width_ / (slices_ - 1), heightB - heightA, 0.0f);
	Vector3f v(0.0f, heightC - heightA, -depth_ / (stacks_ - 1));

	// find normal
	Vector3f n = CrossProduct(u, v);
	n.Normalize();

	float k = DotProduct(n, -dirLight);
	if (k < 0.0f)
		k = 0.0f;
	return k;
}

float Terrain::GetHeight(float x, float z)
{
	float tx = x + (float)width_ / 2.0f;
	float tz = z + (float)depth_ / 2.0f;

	float xcell = width_ / (slices_ - 1);
	float zcell = depth_ / (stacks_ - 1);

	// 1. compute row & col(notes here we count left bottom as(0,0))
	uint32 row = floor2int(tz / zcell);
	uint32 col = floor2int(tx / xcell);

	// 2. get the heights of the quad we're in:
	//  C   D
	//  *---*
	//  | / |
	//  *---*  
	//  A   B
	uint32 heightmapRowIndex = stacks_ - 1 - row;
	uint32 heightmapColIndex = col;
	float A = GetHeightmapEntry(heightmapRowIndex, heightmapColIndex);
	float B = GetHeightmapEntry(heightmapRowIndex, heightmapColIndex + 1);
	float C = GetHeightmapEntry(heightmapRowIndex + 1, heightmapColIndex);
	float D = GetHeightmapEntry(heightmapRowIndex + 1, heightmapColIndex + 1);

	// 3. get triangle we're in (ACD or ADB?)
	// in cell (row, col), we compute dx, dz
	float dx = tx / xcell - col;
	float dz = tz / zcell - row;
	
	float height = 0.0f;
	if (dx + dz > 1) // ACD
	{
		float ux = D - C;
		float uz = A - C;
		height = C + lerp_t(dx, 0.0f, ux) + lerp_t(1.0f - dz, 0.0f, uz);
	}
	else // ADB
	{
		float ux = A - B;
		float uz = D - B;
		height = B + lerp_t(1.0f - dx, 0.0f, ux) + lerp_t(dz, 0.0f, uz);
	}
	return height;
}

float Terrain::ComputeStandAngle(float x, float z)
{
	float tx = x + (float)width_ / 2.0f;
	float tz = z + (float)depth_ / 2.0f;

	float xcell = width_ / (slices_ - 1);
	float zcell = depth_ / (stacks_ - 1);

	// 1. compute row & col(notes here we count left bottom as(0,0))
	uint32 row = floor2int(tz / zcell);
	uint32 col = floor2int(tx / xcell);

	// 2. get 4 position of the quad we're in:
	//  C   D
	//  *---*
	//  | / |
	//  *---*  
	//  A   B

	auto& posA = vertices_[row * slices_ + col].p;
	auto& posB = vertices_[row * slices_ + col + 1].p;
	auto& posC = vertices_[(row + 1) * slices_ + col].p;
	auto& posD = vertices_[(row + 1) * slices_ + col + 1].p;

	// 3. get triangle we're in (ACD or ADB?)
	// in cell (row, col), we compute dx, dz
	float dx = tx / xcell - col;
	float dz = tz / zcell - row;
	Vector3f n;
	if (dx + dz > 1) // ACD
	{
		// 4. compute triangle normal
		Vector3f u = posC - posA;
		Vector3f v = posD - posA;
		n = CrossProduct(u, v);

	}
	else // ADB
	{
		Vector3f u = posD - posA;
		Vector3f v = posB - posA;
		n = CrossProduct(u, v);
	}
	n.Normalize();

	Vector3f nxz(0.0f, 1.0f, 0.0f);
	float f = (DotProduct(n, nxz));
	return radian2angle(acos(f));
}