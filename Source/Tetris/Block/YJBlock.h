#pragma once

#include "CoreMinimal.h"

enum class EYJBlockType : uint8
{
	None, I, L, J, O, S, T, Z,
	Num,
};

enum class EYJMoveDirection : uint8
{
	Left, Right, Down,
	Num,
};

class FYJTetrisBlock;

///////////////////////////////////////////////////////////////////////////////////////////////////

class TETRIS_API FYJBlockStatics
{
public:
	static const TMap<EYJMoveDirection, FIntPoint> MoveDirectionMap;

	static const FColor& GetBlockNoneColor();
	static const TMap<EYJBlockType, FColor> BlockColorMap;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * 블록 객체 (블록 그리드, 테트리스 블록의 한 부분을 나타냄)
 * 
 */
struct TETRIS_API FYJBlock
{
public:
	FIntPoint Position;
	FColor Color;

public:
	FYJBlock() = default;
	FYJBlock(const FIntPoint& InPosition, const FColor& InColor);
};

///////////////////////////////////////////////////////////////////////////////////////////////////


/**
* 테트리스 블록
* 
*/
class TETRIS_API FYJTetrisBlock
{
public:
	static const uint32 BlockNum = 4;

public:
	FYJTetrisBlock() = default;

	FYJTetrisBlock(EYJBlockType InBlockType, const FIntPoint& InStartPos);

	void CreateBlocks();

	void Move(EYJMoveDirection MoveDirection);
	void Rotate(const FQuat2D& Rotation);

public:
	const FColor& GetColor() const;

	void SetPosition(const FIntPoint& NewPosition) { Position = NewPosition; }
	const FIntPoint& GetPosition() const { return Position; }
		
	TArray<FIntPoint> GetBlockPositions() const;

private:
	TStaticArray<FYJBlock, BlockNum> Blocks;
	EYJBlockType BlockType;
	FIntPoint Position;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * 블록 그리드
 * 좌상 (0,0) / 우하(Column, Row)
 */
class TETRIS_API FYJBlockGrid
{
public:
	FYJBlockGrid() = default;

	void Init(int32 InRow, int32 InColumn);

	bool RequestMove(FYJTetrisBlock& TetrisBlock, EYJMoveDirection MoveDirection);
	bool RequestDrop(FYJTetrisBlock& TetrisBlock);
	bool RequestRotate(FYJTetrisBlock& TetrisBlock, bool bClockWise = true);
	bool RequestDeleteLine();
	
	bool IsNoneBlockPosition(const FIntPoint& Position) const;
	bool IsBlockExist(const FYJTetrisBlock& TetrisBlock) const;

	void ClearAllBlocks();

	FIntPoint GetBlockStartPosition() const { return FIntPoint(Column / 2.0f, 0); }

public:
	DECLARE_EVENT_OneParam(FYJBlockGrid, FOnBlockRefreshed, const TArray<FYJBlock>& /** RefreshedBlocks*/);
	FOnBlockRefreshed& OnBlockRefreshed() const { return OnBlockRefreshedEvent; }

protected:
	void CreateGrid();

	void SetColor(const TArray<FIntPoint>& Positions, const FColor& NewColor);

	void Refresh(const TArray<FIntPoint>& NonedPositions, const TArray<FIntPoint>& NewPositions, const FColor& NewColor);
	void BroadcastAllBlockRefreshed() const;

private:
	bool IsGridOut(const FIntPoint& Position) const;
	const FYJBlock* GetBlock(const FIntPoint& Position) const;
	FYJBlock* GetBlock(const FIntPoint& Position);

	bool CheckBlockCanMoveTo(const TArray<FIntPoint>& CurrBlockPositions, const TArray<FIntPoint>& BlockPositionsToMove) const;
	bool MoveInternal(FYJTetrisBlock& TetrisBlock, EYJMoveDirection MoveDirection, bool bOnceMove);

private:
	static const FQuat2D& GetRotation90Degree(bool bClockWise = true);

private:
	int32 Row, Column;
	TArray<TArray<FYJBlock>> Block2DArray;

	mutable FOnBlockRefreshed OnBlockRefreshedEvent;
};
