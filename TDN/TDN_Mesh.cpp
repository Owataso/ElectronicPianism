
#include "TDNLIB.h"

tdnMesh::tdnMesh() : decl( nullptr ), declSize( 0 ), vertexBuffer( nullptr ), numVertexes( 0 ), indexBuffer( nullptr ), numIndexes( 0 ), streamBuffer( nullptr ), streamSize( 0 ), numStream( 0 ), numFaces( 0 ), pos( 0, 0, 0 ), scale( 1, 1, 1 ), rot( 0, 0, 0, 1 ), texture( nullptr )
{
	D3DXMatrixIdentity( &worldMatrix );
}
tdnMesh::~tdnMesh()
{
	if( decl )
		decl->Release();
	if( vertexBuffer )
		vertexBuffer->Release();
	if( indexBuffer )
		indexBuffer->Release();
}

/********/
/* 作成 */
/********/

bool tdnMesh::CreateVertex( unsigned int numVertexes, unsigned int vertexSize, void *vertexArray )
{
	HRESULT hr( S_OK );

	// 頂点配列のサイズ計算
	unsigned int vertexesSize = vertexSize * numVertexes;
	this->numVertexes = numVertexes;

	// バッファnew
	hr = tdnSystem::GetDevice()->CreateVertexBuffer(
		vertexesSize,    // 頂点配列のバイト数
		0,               // 使用方法 D3DUSAGE
		D3DFVF_MESHVERTEX2,      // ?
		D3DPOOL_MANAGED, // 保存したデータの使用方法（読み込み専用, 最適化する ...）
		&vertexBuffer,   // 頂点バッファ <out>
		0 );             // ?
	if( FAILED( hr ) )
		return false;

	// バッファにコピー
	void* workVertex( nullptr );
	hr = vertexBuffer->Lock( 0, 0, ( void** ) &workVertex, 0 );      // バッファロック (バファのポインター取得)
	if( FAILED( hr ) )
		return false;
	memcpy_s( workVertex, vertexesSize, vertexArray, vertexesSize ); // バッファに頂点情報をコピー
	hr = vertexBuffer->Unlock();                                     // バッファロックを解除
	if( FAILED( hr ) )
		return false;

	return true;
}
bool tdnMesh::CreateIndexes( unsigned int numIndexes, const DWORD *indexArray )
{
	HRESULT hr( S_OK );

	this->numIndexes = numIndexes;

	// インデックス配列のサイズ計算
	unsigned int indexesSize = sizeof( DWORD ) * numIndexes;
	// インデックスnew
	hr = tdnSystem::GetDevice()->CreateIndexBuffer(
		indexesSize,     // インデックス配列のバイト数
		0,               // 使用方法 D3DUSAGE
		D3DFMT_INDEX32,  // フォーマット
		D3DPOOL_MANAGED, // ?
		&indexBuffer,    // インデックスバッファ <out>
		NULL );          // ?
	if( FAILED( hr ) )
		return false;

	// バッファにコピー
	DWORD* workIndex( nullptr );
	hr = indexBuffer->Lock( 0, 0, ( void** ) &workIndex, 0 );    // バッファロック (バファのポインター取得)
	if( FAILED( hr ) )
		return false;
	memcpy_s( workIndex, indexesSize, indexArray, indexesSize ); // バッファにインデックス情報をコピー
	hr = indexBuffer->Unlock();                                  // バッファロックを解除
	if( FAILED( hr ) )
		return false;

	return true;
}
bool tdnMesh::CreateStream( unsigned int numData, unsigned int dataSize, void *dataArray )
{
	HRESULT hr( S_OK );

	// 頂点配列のサイズ計算
	unsigned int dataArraySize = dataSize * numData;
	streamSize = dataSize;
	numStream = numData;

	// バッファnew
	hr = tdnSystem::GetDevice()->CreateVertexBuffer(
		dataArraySize,   // 配列のバイト数
		0,               // 使用方法 D3DUSAGE
		0,               // ?
		D3DPOOL_MANAGED, // 保存したデータの使用方法（読み込み専用, 最適化する ...）
		&streamBuffer,   // バッファ <out>
		0 );             // ?
	if( FAILED( hr ) )
		return false;

	// バッファにコピー
	void* workData( nullptr );
	hr = streamBuffer->Lock( 0, 0, ( void** ) &workData, 0 );      // バッファロック (バファのポインター取得)
	if( FAILED( hr ) )
		return false;
	memcpy_s( workData, dataArraySize, dataArray, dataArraySize ); // バッファに頂点情報をコピー
	hr = streamBuffer->Unlock();                                     // バッファロックを解除
	if( FAILED( hr ) )
		return false;

	return true;
}
bool tdnMesh::CreateDeclaration( unsigned int declSize, D3DVERTEXELEMENT9 *decl )
{
	HRESULT hr( S_OK );

	this->declSize = declSize;
	//FVF作成
	hr = tdnSystem::GetDevice()->CreateVertexDeclaration( decl, &this->decl );
	if( FAILED( hr ) )
		return false;

	return true;
}
bool tdnMesh::Create( const CreateData &data )
{
	if( CreateVertex( data.numVertexes, data.vertexSize, data.vertexArray ) == false )
		return false;
	if( data.numIndexes != 0 && data.indexArray != nullptr)
		if( CreateIndexes( data.numIndexes, data.indexArray ) == false )
			return false;
	if( data.streamSize != 0 && data.streamArray != nullptr && data.numStream != 0 )
		if( CreateStream( data.numStream, data.streamSize, data.streamArray ) == false )
			return false;
	if( CreateDeclaration( data.vertexSize, data.decl ) == false )
		return false;
	numFaces = data.numIndexes / 3;
	return true;
}

bool tdnMesh::CreateTriangle( float radius, DWORD color )
{
	CreateData data;
	float rad120( PI * 2 / 3 );
	float s( sinf( rad120 ) ), c( cosf( rad120 ) );
	VECTOR_LINE vertexArray[]
	{
		{ -s*radius, c*radius, 0, color },
		{ 0, radius, 0, color },
		{ s*radius, c*radius, 0, color }
	};
	data.vertexArray = vertexArray;
	data.vertexSize = sizeof( VECTOR_LINE );
	data.numVertexes = ARRAYSIZE( vertexArray );

	DWORD indexArray[]
	{
		0, 1, 2
	};
	data.indexArray = indexArray;
	data.numIndexes = ARRAYSIZE( indexArray );

	D3DVERTEXELEMENT9 declAry[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, sizeof( Vector3 ), D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};
	data.decl = declAry;

	return Create( data );
}
bool tdnMesh::CreateRectangle( float width, float height, DWORD color )
{
	CreateData data;
	width *= 0.5f;
	height *= 0.5f;
	VECTOR_LINE vertexArray[]
	{
		{ -width, -height, 0, color},
		{ -width, height, 0, color },
		{ width, -height, 0, color },
		{ width, height, 0, color }
	};
	data.vertexArray = vertexArray;
	data.vertexSize = sizeof( VECTOR_LINE );
	data.numVertexes = ARRAYSIZE( vertexArray );

	DWORD indexArray[]
	{
		0, 1, 2,
			1, 3, 2
	};
	data.indexArray = indexArray;
	data.numIndexes = ARRAYSIZE( indexArray );

	D3DVERTEXELEMENT9 declAry[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, sizeof( Vector3 ), D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};
	data.decl = declAry;

	return Create( data );
}
bool tdnMesh::CreateTriangular( float radius, DWORD color )
{
	CreateData data;
	float rad120( PI * 2 / 3 );
	float s( sinf( rad120 ) ), c( cosf( rad120 ) );
	VECTOR_LINE vertexArray[]
	{
		{0, radius, 0, color},
		{ 0, c*radius, radius, color },
		{ -s*radius, c*radius, c*radius, color },
		{ s*radius, c*radius, c*radius, color }
	};
	data.vertexArray = vertexArray;
	data.vertexSize = sizeof( VECTOR_LINE );
	data.numVertexes = ARRAYSIZE( vertexArray );

	DWORD indexArray[]
	{
		0, 3, 2,
			0, 1, 3,
			0, 2, 1,
			1, 2, 3,
	};
	data.indexArray = indexArray;
	data.numIndexes = ARRAYSIZE( indexArray );

	D3DVERTEXELEMENT9 declAry[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, sizeof( Vector3 ), D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};
	data.decl = declAry;

	return Create( data );
}
bool tdnMesh::CreateCube( float width, float height, float depth, DWORD color, Vector3 *posList, unsigned int numPos )
{
	CreateData data;
	width *= 0.5f;
	height *= 0.5f;
	depth *= 0.5f;
	VECTOR_LINE vertexArray[]
	{
		{ -width, -height, -depth, color},
		{ -width, -height, depth, color },
		{ -width, height, -depth, color },
		{ -width, height, depth, color },
		{ width, -height, -depth, color },
		{ width, -height, depth, color },
		{ width, height, -depth, color },
		{ width, height, depth, color }
	};
	data.vertexArray = vertexArray;
	data.vertexSize = sizeof( VECTOR_LINE );
	data.numVertexes = ARRAYSIZE( vertexArray );

	DWORD indexArray[]
	{
		0, 2, 6,
			0, 6, 4,
			4, 6, 7,
			4, 7, 5,
			3, 5, 7,
			1, 5, 3,
			1, 3, 2,
			2, 0, 1,
			0, 4, 1,
			4, 5, 1,
			2, 3, 7,
			7, 6, 2
	};
	data.indexArray = indexArray;
	data.numIndexes = ARRAYSIZE( indexArray );

	if( numPos > 0 )
	{
		D3DVERTEXELEMENT9 declAry[] = {
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, sizeof( Vector3 ), D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			// streamBuffer インスタンシング
			{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
			D3DDECL_END()
		};
		data.decl = declAry;
	}
	else
	{
		D3DVERTEXELEMENT9 declAry[] = {
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, sizeof( Vector3 ), D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			D3DDECL_END()
		};
		data.decl = declAry;
	}

	data.streamArray = posList;
	data.streamSize = sizeof( Vector3 );
	data.numStream = numPos;

	return Create( data );
}

/************/
/* 読み込み */
/************/

template<class TYPE_FSTREAM> bool OpenFstream( TYPE_FSTREAM *out, std::string filename )
{
	out->open( filename );
	if( out->fail() ) // 開けなかった
	{
		char text[256] = {};
		sprintf_s(text, 256, "%s が開けません", filename.c_str());
		MessageBox( NULL, text, "エラー", 0 );
		return false;
	}
	return true;
}

// MQOファイル

class MqoLoadObject
{
public:
	virtual void Load( std::ifstream &mqoFile ) = 0;

protected:
	// LoadLoopで使う関数（関数ポインター）の戻り値
	enum class FPCODE : byte
	{
		FAILURE, // 失敗
		SUCCESS, // 成功
		END      // ループ終了
	};

	// " " の間を取り出す
	inline void LoadName(
		std::ifstream &mqoFile, // <in> : 読み込むファイル
		std::string &name       // <out>: 読み込んだ文字列から " " の間を取り出したもの
		)
	{
		mqoFile >> name;
		name = name.substr( 1, name.size() - 2 ); // 前後の " " を消す
	}

	template< class type, // <省略可> outの形
		size_t size>      // <省略可> 配列の要素数
		// type の配列を読み込む
		inline FPCODE LoadArray(
		std::ifstream &mqoFile,  // <in> : 読み込むファイル
		const std::string& load, // <in> : 前回読み込んだ文字列
		const std::string& com,  // <in> : 判定用文字列
		type( &out )[size]       // <out>: ここに読み込む
		)
	{
		// 終了
		if( load == "}" )
			return FPCODE::END;
		// 違う
		if( load != com )
			return FPCODE::FAILURE;
		// 配列分読み込み
		for( size_t i = 0; i < size; i++ )
		{
			mqoFile >> out[i];
		}
		return FPCODE::SUCCESS;
	}

	void sto( const std::string in, int *out )
	{
		*out = std::stoi( in );
	}
	void sto( const std::string in, unsigned int *out )
	{
		*out = std::stoi( in );
	}
	void sto( const std::string in, float *out )
	{
		*out = std::stof( in );
	}

	template <class type> // <省略可> 取り出す型
	// com(type) の形の文字列を読み込みtypeの取り出す
	inline FPCODE LoadParenthesi(
		const std::string& load, // <in> : 前回読み込んだ文字列
		const std::string com,   // <in> : 判定用文字列
		type *out                // <out>: ここに読み込む
		)
	{
		// 終了
		if( load == "}" )
			return FPCODE::END;
		// 違う
		if( load.substr( 0, com.size() ) != com )
			return FPCODE::FAILURE;
		// 取り出す
		sto( load.substr( com.size() + 1 ), out );
		return FPCODE::SUCCESS;
	}

	// com("string") の形の文字列を読み込みstringを取り出す
	inline FPCODE LoadStringParenthesi(
		const std::string& load, // <in> : 前回読み込んだ文字列
		const std::string com,   // <in> : 判定用文字列
		std::string* out         // <out>: ここに読み込む
		)
	{
		// 終了
		if( load == "}" )
			return FPCODE::END;
		// 違う
		if( load.substr( 0, com.size() ) != com )
			return FPCODE::FAILURE;
		// 取り出す
		*out = load.substr( com.size() + 2, load.size() - com.size() - 4 );
		return FPCODE::SUCCESS;
	}

	template<class type, // 取り出す型
		size_t size>     // 配列の要素数
		// com(type type ...) の形の文字列を読み込みtypeの配列を取り出す
		inline FPCODE LoadArrayParenthesi(
		std::ifstream& mqoFile, // <in> : 読み込むファイル
		std::string& load,      // <in> : 前回読み込んだ文字列
		const std::string com,  // <in> : 判定用文字列
		type( &out )[size]      // <out>: ここに読み込む
		)
	{
		// 終了
		if( load == "}" )
			return FPCODE::END;
		// 違う
		if( load.substr( 0, com.size() ) != com )
			return FPCODE::FAILURE;
		// 数字を取り出す
		out[0] = stof( load.substr( com.size() + 1 ) );
		for( size_t i = 1; i < size; i++ )
		{
			mqoFile >> out[i];
		}
		std::string workString;
		mqoFile >> workString; // ")" を読む
		return FPCODE::SUCCESS;
	}

	template<class LoadObjectClass> // MqoLoadObject を継承したクラス 
	// 関数ポインターの配列を回す
	void LoadLoop(
		LoadObjectClass* me,    // <in> : thisポインター入れる
		std::ifstream &mqoFile, // <in> : 読み込むファイル
		FPCODE( LoadObjectClass::*loadFP[] )( std::ifstream&, std::string& load ), // <in> : 関数ポインタ配列
		unsigned int numFP      // <in> : 配列お要素数
		)
	{
		for( unsigned int step( 0 ); step < numFP; )
		{
			std::string workString;
			mqoFile >> workString;
			if( mqoFile.eof() )
				break;
			// 関数ポインター分回す
			for( unsigned int i = step; i < numFP; i++ )
			{
				switch( ( me->*loadFP[i] )( mqoFile, workString ) )
				{
					case FPCODE::FAILURE: // 違った
						break;

					case FPCODE::SUCCESS: // 読み込んだ
						step = i;         // stepを進める
						i = numFP;        // ループ一つ抜ける
						break;

					case FPCODE::END: // ループ終了
						i = numFP;    // ループ一つ抜ける
						step = numFP; // ループ抜ける
						break;
				}
			}
		}
	}
};

class MqoMaterial : public MqoLoadObject
{
public:
	std::string name;    // 材質名       31byte以内
	float col[4];        // 頂点色 RGBA  0~1
	float dif;           // 拡散光       0~1
	float amb;           // 周囲光       0~1
	float emi;           // 自己照明     0~1
	float spc;           // 反射光       0~1
	float power;         // 反射光の強さ 0~100
	float reflect;       // 鏡面反射     0~1
	float refract;       // 屈折率       1~5
	std::string tex;     // 模様テクスチャ
	std::string aplane;  // 透明テクスチャ
	std::string bump;    // 凹凸テクスチャ
	float proj_pos[3];   // 位置 XYZ
	float proj_scale[3]; // 拡大率 XYZ
	float proj_angle[3]; // 角度 YXZ     -180~180

	MqoMaterial() : dif( 0 ), amb( 0 ), emi( 0 ), spc( 0 ), power( 0 ), reflect( 0 ), refract( 0 )
	{
		col[0] = col[1] = col[2] = col[3] = 1;
		proj_pos[0] = proj_pos[1] = proj_pos[2] = 0;
		proj_scale[0] = proj_scale[1] = proj_scale[2] = 1;
		proj_angle[0] = proj_angle[1] = proj_angle[2] = 0;
	}

	void Load( std::ifstream &mqoFile )
	{
		std::string workString;
		mqoFile >> workString; // { を読む
		LoadName( mqoFile, name );

		// 変数を読み込む関数
		FPCODE( MqoMaterial::*loadFP[] )( std::ifstream &mqoFile, std::string& load )
		{
			&MqoMaterial::LaodCol, &MqoMaterial::LoadDif, &MqoMaterial::LoadAmb, &MqoMaterial::LoadEmi, &MqoMaterial::LoadSpc, &MqoMaterial::LoadPower, &MqoMaterial::LoadReflect, &MqoMaterial::LoadRefract, &MqoMaterial::LoadTex, &MqoMaterial::LoadAplane, &MqoMaterial::LoadBump, &MqoMaterial::LaodProj_pos, &MqoMaterial::LaodProj_scale, &MqoMaterial::Laodproj_angle
		};

		LoadLoop<MqoMaterial>( this, mqoFile, loadFP, ARRAYSIZE( loadFP ) );
	}

private:
	FPCODE LaodCol( std::ifstream &mqoFile, std::string& load )
	{
		return LoadArrayParenthesi( mqoFile, load, "col", col );
	}
	FPCODE LoadDif( std::ifstream &mqoFile, std::string& load )
	{
		return LoadParenthesi( load, "dif", &dif );
	}
	FPCODE LoadAmb( std::ifstream &mqoFile, std::string& load )
	{
		return LoadParenthesi( load, "amb", &amb );
	}
	FPCODE LoadEmi( std::ifstream &mqoFile, std::string& load )
	{
		return LoadParenthesi( load, "emi", &emi );
	}
	FPCODE LoadSpc( std::ifstream &mqoFile, std::string& load )
	{
		return LoadParenthesi( load, "spc", &spc );
	}
	FPCODE LoadPower( std::ifstream &mqoFile, std::string& load )
	{
		return LoadParenthesi( load, "power", &power );
	}
	FPCODE LoadReflect( std::ifstream &mqoFile, std::string& load )
	{
		return LoadParenthesi( load, "reflect", &reflect );
	}
	FPCODE LoadRefract( std::ifstream &mqoFile, std::string& load )
	{
		return LoadParenthesi( load, "refract", &refract );
	}
	FPCODE LoadTex( std::ifstream &mqoFile, std::string& load )
	{
		return LoadStringParenthesi( load, "tex", &tex );
	}
	FPCODE LoadAplane( std::ifstream &mqoFile, std::string& load )
	{
		return LoadStringParenthesi( load, "aplane", &aplane );
	}
	FPCODE LoadBump( std::ifstream &mqoFile, std::string& load )
	{
		return LoadStringParenthesi( load, "bump", &bump );
	}
	FPCODE LaodProj_pos( std::ifstream &mqoFile, std::string& load )
	{
		return LoadArrayParenthesi( mqoFile, load, "proj_pos", proj_pos );
	}
	FPCODE LaodProj_scale( std::ifstream &mqoFile, std::string& load )
	{
		return LoadArrayParenthesi( mqoFile, load, "proj_scale", proj_scale );
	}
	FPCODE Laodproj_angle( std::ifstream &mqoFile, std::string& load )
	{
		return LoadArrayParenthesi( mqoFile, load, "proj_angle", proj_angle );
	}
};

class MqoVertexList : public MqoLoadObject
{
public:
	class vertex
	{
	public:
		float pos[3];

		vertex()
		{
			pos[0] = pos[1] = pos[2] = 0;
		}

		void Load( std::ifstream& mqoFile )
		{
			mqoFile >> pos[0];
			mqoFile >> pos[1];
			mqoFile >> pos[2];
		}
	};

	std::vector<vertex> list;

	void Load( std::ifstream &mqoFile )
	{
		unsigned int numVertex;
		mqoFile >> numVertex;
		list.resize( numVertex );

		std::string workString;
		mqoFile >> workString; // { を読み込む

		for( size_t i = 0; i < numVertex; i++ )
		{
			list[i].Load( mqoFile );
		}

		mqoFile >> workString; // } を読み込む
	}
};

class MqoFaceList : public MqoLoadObject
{
public:

	class Face
	{
	public:
		class Vertex
		{
		public:
			unsigned int V;        // 頂点インデックス
			float UV[2];
			DWORD COL;             // 頂点色 ABGR

			Vertex() : V( 0 ), COL( 0xFFFFFFFF )
			{
				UV[0] = UV[1] = 0;
			}
		};
		std::vector<Vertex> vertex; // 頂点インデックスと頂点情報
		unsigned int M;             // 材質インデックス

		Face() : M( 0 )
		{}

		void LoadV( std::ifstream &mqoFile, const std::string& load )
		{
			vertex[0].V = std::stoi( load.substr( 2 ) );
			for( size_t i = 1; i < vertex.size(); i++ )
			{
				mqoFile >> vertex[i].V;
			}
		};
		void LoadM( const std::string& load )
		{
			M = std::stoi( load.substr( 2 ) );
		}
		void LoadUV( std::ifstream &mqoFile, const std::string& load )
		{
			vertex[0].UV[0] = std::stof( load.substr( 3 ) );
			mqoFile >> vertex[0].UV[1];
			for( size_t i = 1; i < vertex.size(); i++ )
			{
				mqoFile >> vertex[i].UV[0];
				mqoFile >> vertex[i].UV[1];
			}
		}
		void LoadCOL( std::ifstream &mqoFile, const std::string& load )
		{
			vertex[0].COL = std::stoi( load.substr( 4 ) );
			for( size_t i = 1; i < vertex.size(); i++ )
			{
				mqoFile >> vertex[i].COL;
			}
		}
	};
	std::vector<Face> list;

	void Load( std::ifstream &mqoFile )
	{
		unsigned int numFace( 0 );
		mqoFile >> numFace;
		list.resize( numFace );

		std::string workString;
		mqoFile >> workString; // { 読み込み

		for( size_t i = 0; i <= list.size(); )
		{
			mqoFile >> workString;
			if( workString == "}" )
				break;

			if( "0" <= workString && workString <= "9" )
			{
				i++;
				list[i - 1].vertex.resize( std::stoi( workString ) );
				continue;
			}

			if( i <= 0 )
				continue;

			if( workString.find( "COL" ) != std::string::npos )
			{
				list[i - 1].LoadCOL( mqoFile, workString );
			}
			else if( workString.find( "UV" ) != std::string::npos )
			{
				list[i - 1].LoadUV( mqoFile, workString );
			}
			else if( workString.find( "V" ) != std::string::npos )
			{
				list[i - 1].LoadV( mqoFile, workString );
			}
			else if( workString.find( "M" ) != std::string::npos )
			{
				list[i - 1].LoadM( workString );
			}
		}

		mqoFile >> workString; // } 読み込み
	}

private:
};

class MqoObject : public MqoLoadObject
{
public:
	std::string name;     // オブジェクト名 63byte以内
	float scale[3];       // 拡大 XYZ
	float rotation[3];    // 回転 YXZ
	float translation[3]; // 移動 XYZ
	int visible;         // 0 : 非表示, 15 : 表示
	float color[3];       // 色   RGB 0~1

	MqoVertexList vertexList;
	MqoFaceList faceList;

	MqoObject()
	{}

	void Load( std::ifstream &mqoFile )
	{
		LoadName( mqoFile, name );

		FPCODE( MqoObject::*loadFP[] )( std::ifstream &mqoFile, std::string& load )
		{
			&MqoObject::LoadScale, &MqoObject::LoadRotation, &MqoObject::LoadtTranslation, &MqoObject::LoadtVisible, &MqoObject::LoadtColor, &MqoObject::LoadVertex, &MqoObject::LoadFace,
		};

		LoadLoop<MqoObject>( this, mqoFile, loadFP, ARRAYSIZE( loadFP ) );
	}

private:
	FPCODE LoadScale( std::ifstream &mqoFile, std::string& load )
	{
		return LoadArray( mqoFile, load, "scale", scale );
	}
	FPCODE LoadRotation( std::ifstream &mqoFile, std::string& load )
	{
		return LoadArray( mqoFile, load, "rotation", rotation );
	}
	FPCODE LoadtTranslation( std::ifstream &mqoFile, std::string& load )
	{
		return LoadArray( mqoFile, load, "translation", translation );
	}
	FPCODE LoadtVisible( std::ifstream &mqoFile, std::string& load )
	{
		if( load == "}" )
			return FPCODE::END;

		if( load != "visible" )
			return FPCODE::FAILURE;

		mqoFile >> visible;
		return FPCODE::SUCCESS;
	}
	FPCODE LoadtColor( std::ifstream &mqoFile, std::string& load )
	{
		return LoadArray( mqoFile, load, "color", color );
	}
	FPCODE LoadVertex( std::ifstream &mqoFile, std::string& load )
	{
		if( load == "}" )
			return FPCODE::END;

		if( load != "vertex" )
			return FPCODE::FAILURE;

		vertexList.Load( mqoFile );
		return FPCODE::SUCCESS;
	}
	FPCODE LoadFace( std::ifstream &mqoFile, std::string& load )
	{
		if( load == "}" )
			return FPCODE::END;

		if( load != "face" )
			return FPCODE::FAILURE;

		faceList.Load( mqoFile );
		return FPCODE::SUCCESS;
	}
};

class MQOLoader
{
public:
	MQOLoader() : vertexList( nullptr ), numVtx( 0 ), indexList( nullptr ), vertexDataList( nullptr ), numIdx( 0 )
	{}
	~MQOLoader()
	{
		SAFE_DELETE( vertexList );
		SAFE_DELETE( indexList );
		SAFE_DELETE( vertexDataList );
	}

	bool CreateMesh( tdnMesh *mesh, std::string filename )
	{
		std::ifstream mqoFile;
		OpenFstream( &mqoFile, filename );
		Load( mqoFile );
		mqoFile.close();

		CreateVertexArray();
		CreateIndexAndDataArray();

		tdnMesh::CreateData data;
		data.numVertexes = numIdx;
		data.vertexSize = sizeof( MESHVERTEX2 );
		data.vertexArray = vertexDataList;
		data.numIndexes = numIdx;
		data.indexArray = indexList;
		data.streamSize = 0;
		data.streamArray = nullptr;

		D3DVERTEXELEMENT9 declAry[] = {
			// vertexBufer
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, sizeof( float ) * 3, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 0, sizeof( float ) * 6, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 0, sizeof( float ) * 6 + sizeof(COLOR), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			D3DDECL_END()
		};
		data.decl = declAry;

		bool ret( true );
		ret = mesh->Create( data );

		std::string filepath;
		float seekl = (float)filename.find_last_of( "/" );
		filepath = filename.substr( 0, (int)seekl + 1 );
		filepath += materialList[0].tex;
		mesh->texture = tdnTexture::Load( "DATA/mqo/box/box.png" );

		return ret;
	}

private:
	struct Vertex
	{
		Vector3 position;
		Vector3 normal;
	};
	Vertex *vertexList;
	unsigned int numVtx;

	DWORD *indexList;
	MESHVERTEX2 *vertexDataList;
	unsigned int numIdx;

	std::vector<MqoMaterial> materialList;
	std::vector<MqoObject> objectList;

	// 読み込む
	void Load( std::ifstream &mqoFile )
	{
		while( true )
		{
			std::string command;
			mqoFile >> command;
			if( mqoFile.eof() )
				break;

			// マテリアル読み込み
			if( command.find( "Material" ) != std::string::npos )
			{
				unsigned int numMaterial;
				mqoFile >> numMaterial;
				materialList.resize( numMaterial );

				for( unsigned int i = 0; i < numMaterial; i++ )
				{
					materialList[i].Load( mqoFile );
				}
			}
			// オブジェクト読み込み
			else if( command.find( "Object" ) != std::string::npos )
			{
				objectList.push_back( MqoObject() );
				objectList.rbegin()->Load( mqoFile );
			}
		}
	}

	// 位置だけ設定
	void CreateVertexArray()
	{
		for( unsigned int i1 = 0; i1 < objectList.size(); i1++ )
		{
			numVtx += objectList[i1].vertexList.list.size();
		}
		SAFE_DELETE( vertexList );
		vertexList = new Vertex[numVtx]
		{};

		for( unsigned int vi = 0; vi < numVtx; )
		{
			for( unsigned int i1 = 0; i1 < objectList.size(); i1++ )
			{
				for( unsigned int i2 = 0; i2 < objectList[i1].vertexList.list.size(); i2++ )
				{
					vertexList[vi].position.x = objectList[i1].vertexList.list[i2].pos[0];
					vertexList[vi].position.y = objectList[i1].vertexList.list[i2].pos[1];
					vertexList[vi].position.z = objectList[i1].vertexList.list[i2].pos[2];
					vi++;
				}
			}
		}
	}

	void CreateIndexAndDataArray()
	{
		// インデックス数
		for( unsigned int i1 = 0; i1 < objectList.size(); i1++ )
		{
			for( unsigned int i2 = 0; i2 < objectList[i1].faceList.list.size(); i2++ ) // 三角ポリゴンに変更
				numIdx += 3 * ( objectList[i1].faceList.list[i2].vertex.size() - 2 );
		}
		// 配列new
		SAFE_DELETE( indexList );
		indexList = new DWORD[numIdx]
		{};
		for( unsigned int i = 0; i < numIdx; i++ )
		{
			indexList[i] = i;
		}
		SAFE_DELETE( vertexDataList );
		vertexDataList = new MESHVERTEX2[numIdx]
		{};

		// 法線計算
		for( unsigned int i1 = 0; i1 < objectList.size(); i1++ )
			for( unsigned int i2 = 0; i2 < objectList[i1].faceList.list.size(); i2++ )
				// 三角ポリゴンで法線計算
				for( unsigned int i3 = 0; i3 + 2 < objectList[i1].faceList.list[i2].vertex.size(); i3++ )
				{
					// インデックス
					unsigned int index[3];
					index[0] = objectList[i1].faceList.list[i2].vertex[i3].V;
					index[1] = objectList[i1].faceList.list[i2].vertex[i3 + 1].V;
					index[2] = objectList[i1].faceList.list[i2].vertex[i3 + 2].V;
					// 頂点
					Vector3 vertex[3];
					vertex[0] = vertexList[index[0]].position;
					vertex[1] = vertexList[index[1]].position;
					vertex[2] = vertexList[index[2]].position;
					// 法線
					Vector3 Normal;
					Normal = Vector3Cross( vertex[2] - vertex[0], vertex[1] - vertex[0] );
					Normal.Normalize();

					vertexList[index[0]].normal += Normal;
					vertexList[index[1]].normal += Normal;
					vertexList[index[2]].normal += Normal;
				}
		// 正規化
		for( unsigned int i1 = 0; i1 < numVtx; i1++ )
			vertexList[i1].normal.Normalize();

		// 頂点配列設定
		for( unsigned int iIndex = 0, i1 = 0; i1 < objectList.size(); i1++ )
			for( unsigned int i2 = 0; i2 < objectList[i1].faceList.list.size(); i2++ )
				for( unsigned int i3 = 0; i3 + 2 < objectList[i1].faceList.list[i2].vertex.size(); i3++ )
				{
					// インデックス
					unsigned int index[3];
					index[0] = objectList[i1].faceList.list[i2].vertex[0].V;
					index[1] = objectList[i1].faceList.list[i2].vertex[i3 + 1].V;
					index[2] = objectList[i1].faceList.list[i2].vertex[i3 + 2].V;

					auto setData = [&]( unsigned int index1, unsigned int index2, unsigned int index3 )
					{
						vertexDataList[index1].x = vertexList[index2].position.x;
						vertexDataList[index1].y = vertexList[index2].position.y;
						vertexDataList[index1].z = vertexList[index2].position.z;
						vertexDataList[index1].nx = vertexList[index2].normal.x;
						vertexDataList[index1].ny = vertexList[index2].normal.y;
						vertexDataList[index1].nz = vertexList[index2].normal.z;

						vertexDataList[index1].tu = objectList[i1].faceList.list[i2].vertex[index3].UV[0];
						vertexDataList[index1].tv = objectList[i1].faceList.list[i2].vertex[index3].UV[1];
						vertexDataList[index1].color = objectList[i1].faceList.list[i2].vertex[index3].COL;
					};
					// 頂点
					setData( iIndex, index[0], 0 );
					setData( iIndex + 2, index[1], i3 + 1 );
					setData( iIndex + 1, index[2], i3 + 2 );
					iIndex += 3;
				}

	}

};

bool tdnMesh::LoadMqo( char *filename )
{
	std::ifstream mqoFile;
	if( OpenFstream( &mqoFile, filename ) == false )
		return false;

	MQOLoader loader;
	loader.CreateMesh( this, filename );

	return true;
}

/********/
/* 更新 */
/********/

void tdnMesh::UpdateWorldMatrix()
{
	D3DXMatrixRotationQuaternion( &worldMatrix, &rot );
	worldMatrix._11 *= scale.x;
	worldMatrix._12 *= scale.x;
	worldMatrix._13 *= scale.x;
	worldMatrix._14 *= scale.x;
	worldMatrix._21 *= scale.y;
	worldMatrix._22 *= scale.y;
	worldMatrix._23 *= scale.y;
	worldMatrix._24 *= scale.y;
	worldMatrix._31 *= scale.z;
	worldMatrix._32 *= scale.z;
	worldMatrix._33 *= scale.z;
	worldMatrix._34 *= scale.z;
	worldMatrix._41 = pos.x;
	worldMatrix._42 = pos.y;
	worldMatrix._43 = pos.z;
	worldMatrix._44 = 1;
}

/********/
/* 描画 */
/********/

void tdnMesh::Render( tdnShader *shader, char *technique )
{
	unsigned int numPass = shader->Begin( technique );

	shader->SetValue( "worldMatrix", worldMatrix );
	if( texture )
		shader->SetValue( "Texture", texture );

	//使用ストリーム指定と頂点バッファ設定
	tdnSystem::GetDevice()->SetStreamSource( 0, vertexBuffer, 0, declSize );
	tdnSystem::GetDevice()->SetStreamSourceFreq( 0, D3DSTREAMSOURCE_INDEXEDDATA | numStream ); // ４回描画する
	if( numStream > 0 )
	{
		tdnSystem::GetDevice()->SetStreamSource( 1, streamBuffer, 0, streamSize );
		tdnSystem::GetDevice()->SetStreamSourceFreq( 1, D3DSTREAMSOURCE_INSTANCEDATA | 1 ); // １回描画する毎に次のデータに変更
	}
	//頂点インデックス指定
	bool isIndex = indexBuffer != null;
	tdnSystem::GetDevice()->SetIndices( indexBuffer );
	//頂点構造体指定
	tdnSystem::GetDevice()->SetVertexDeclaration( decl );
	for( unsigned int i = 0; i < numPass; i++ )
	{
		shader->BeginPass( i );

		//描画(面二つと０から始まる頂点と指定)
		if( isIndex )
		tdnSystem::GetDevice()->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0, //開始地点からの相対位置
			0, //最小インデックス番号
			numIndexes, //最大インデックス番号
			0, //インデックス配列の読み取り位置	
			numFaces //面の数
			);
		else
			tdnSystem::GetDevice()->DrawPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			numFaces //面の数
			);
		shader->EndPass();
	}

	shader->End();
}

/*********************/
/* セッター ゲッター */
/*********************/

const Vector3& tdnMesh::Pos()
{
	return pos;
}
void tdnMesh::Pos( const Vector3& in )
{
	pos = in;
}
const Vector3& tdnMesh::Scale()
{
	return scale;
}
void tdnMesh::Scale( const Vector3& in )
{
	scale = in;
}
const Quaternion& tdnMesh::Rot()
{
	return rot;
}
void tdnMesh::Rot( const Quaternion& in )
{
	rot = in;
}
const Matrix& tdnMesh::WorldMatrix()
{
	return worldMatrix;
}
void tdnMesh::WorldMatrix( const Matrix& in )
{
	worldMatrix = in;
}
