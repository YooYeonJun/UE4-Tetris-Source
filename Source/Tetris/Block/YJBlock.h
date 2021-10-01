#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EYJBlockType : int8
{
	None = -1,
	I = 0, L, J, O, S, T, Z,
	Num,
};

UENUM()
enum class EYJMoveDirection : uint8
{
	Left = 0, Right, Down,
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
class TETRIS_API FYJBlock
{
public:
	FYJBlock() = default;
	virtual ~FYJBlock() = default;
	FYJBlock(const FIntPoint& InPosition, const FColor& InColor);

	const FIntPoint& GetPosition() const { return Position; }
	void SetPosition(const FIntPoint& InPosition) { Position = InPosition; }

	const FColor& GetColor() const { return Color; }
	void SetColor(const FColor& InColor) { Color = InColor; }

private:
	FIntPoint Position;
	FColor Color;
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
	virtual ~FYJTetrisBlock() = default;

	FYJTetrisBlock(EYJBlockType InBlockType, FIntPoint InStartPos);

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
	virtual ~FYJBlockGrid() = default;

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
	DECLARE_EVENT_OneParam(FYJBlockGrid, FOnBlockRefreshed, const TArray<TSharedRef<FYJBlock>>& /** RefreshedBlocks*/);
	FOnBlockRefreshed& OnBlockRefreshed() const { return OnBlockRefreshedEvent; }

protected:
	void CreateGrid();

	void SetColor(const TArray<FIntPoint>& Positions, const FColor& NewColor);

	void Refresh(const TArray<FIntPoint>& NonedPositions, const TArray<FIntPoint>& NewPositions, const FColor& NewColor);
	void BroadcastAllBlockRefreshed() const;

private:
	bool IsGridOut(const FIntPoint& Position) const;
	TOptional<TSharedRef<const FYJBlock>> GetBlock(const FIntPoint& Position) const;
	TOptional<TSharedRef<FYJBlock>> GetBlock(const FIntPoint& Position);

	bool CheckBlockCanMoveTo(const TArray<FIntPoint>& CurrBlockPositions, const TArray<FIntPoint>& BlockPositionsToMove) const;
	bool MoveInternal(FYJTetrisBlock& TetrisBlock, EYJMoveDirection MoveDirection, bool bOnceMove);

private:
	static const FQuat2D& GetRotation90Degree(bool bClockWise = true);

private:
	int32 Row, Column;
	TArray<TArray<TSharedRef<FYJBlock>>> Block2DArray;

	mutable FOnBlockRefreshed OnBlockRefreshedEvent;
};