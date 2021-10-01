#include "YJBlock.h"
#include "../Tetris.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
// FYJBlockStatics

const TMap<EYJBlockType, FColor> FYJBlockStatics::BlockColorMap
{
	{EYJBlockType::None, FColor::Black},
	{EYJBlockType::I, FColor::Red},
	{EYJBlockType::L, FColor::Purple},
	{EYJBlockType::J, FColor::Yellow},
	{EYJBlockType::O, FColor::Cyan},
	{EYJBlockType::S, FColor::Blue},
	{EYJBlockType::T, FColor::White},
	{EYJBlockType::Z, FColor::Green},
};

const FColor& FYJBlockStatics::GetBlockNoneColor()
{
	return BlockColorMap[EYJBlockType::None];
}

const TMap<EYJMoveDirection, FIntPoint> FYJBlockStatics::MoveDirectionMap
{
	{EYJMoveDirection::Down, FIntPoint{0, 1}},
	{EYJMoveDirection::Left, FIntPoint{-1, 0}},
	{EYJMoveDirection::Right, FIntPoint{1, 0}}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// FYJBlock

FYJBlock::FYJBlock(const FIntPoint& InPosition, const FColor& InColor)
	:Position(InPosition), Color(InColor)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// FYJTetrisBlock

const FColor& FYJTetrisBlock::GetColor() const
{
	return FYJBlockStatics::BlockColorMap[BlockType];
}

TArray<FIntPoint> FYJTetrisBlock::GetBlockPositions() const
{
	TArray<FIntPoint> BlockPositions;
	
	BlockPositions.Reserve(BlockNum);
	for (int32 i = 0; i < BlockNum; i++)
	{
		BlockPositions.Add({ Blocks[i].GetPosition() + GetPosition() });
	}

	return BlockPositions;
}


FYJTetrisBlock::FYJTetrisBlock(EYJBlockType InBlockType, FIntPoint InStartPos)
	:BlockType(InBlockType), Position(InStartPos)
{
	CreateBlocks();
}

void FYJTetrisBlock::CreateBlocks()
{
	// BlockType 에 따라 만든다.
	FIntPoint BlockPositions[BlockNum];

	switch (BlockType)
	{
	case EYJBlockType::I:
		BlockPositions[0] = { -2,  0 };
		BlockPositions[1] = { -1,  0 };
		BlockPositions[2] = {  0,  0 };
		BlockPositions[3] = {  1,  0 };
		break;
	case EYJBlockType::L:
		BlockPositions[0] = {  0, -1 };
		BlockPositions[1] = {  0,  0 };
		BlockPositions[2] = {  0,  1 };
		BlockPositions[3] = {  1,  1 };
		break;
	case EYJBlockType::J:
		BlockPositions[0] = {  0, -1 };
		BlockPositions[1] = {  0,  0 };
		BlockPositions[2] = {  0,  1 };
		BlockPositions[3] = { -1,  1 };
		break;
	case EYJBlockType::O:
		BlockPositions[0] = {  0,  0 };
		BlockPositions[1] = {  1,  0 };
		BlockPositions[2] = {  0,  1 };
		BlockPositions[3] = {  1,  1 };
		break;
	case EYJBlockType::S:
		BlockPositions[0] = {  0,  0 };
		BlockPositions[1] = {  1,  0 };
		BlockPositions[2] = { -1,  1 };
		BlockPositions[3] = {  0,  1 };
		break;
	case EYJBlockType::T:
		BlockPositions[0] = { -1,  0 };
		BlockPositions[1] = {  0,  0 };
		BlockPositions[2] = {  1,  0 };
		BlockPositions[3] = {  0,  1 };
		break;
	case EYJBlockType::Z:
		BlockPositions[0] = { -1, -1 };
		BlockPositions[1] = {  0, -1 };
		BlockPositions[2] = {  0,  0 };
		BlockPositions[3] = {  1,  0 };
		break;

	case EYJBlockType::None:
	case EYJBlockType::Num:
		break;
	default:
		break;
	}

	for (int32 i = 0; i < BlockNum; i++)
	{
		Blocks[i].SetPosition(BlockPositions[i]);
		Blocks[i].SetColor(GetColor());
	}
}

void FYJTetrisBlock::Move(EYJMoveDirection MoveDirection)
{
	Position += FYJBlockStatics::MoveDirectionMap[MoveDirection];
}

void FYJTetrisBlock::Rotate(const FQuat2D& Rotation)
{
	FIntPoint Center;
	for (auto& Block : Blocks)
	{
		Center += Block.GetPosition();
	}
	Center /= BlockNum;

	for (auto& Block : Blocks)
	{
		Block.SetPosition(Rotation.TransformPoint(FVector2D(Block.GetPosition())).IntPoint());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// FYJBlockGrid

void FYJBlockGrid::Init(int32 InRow, int32 InColumn)
{
	Row = InRow;
	Column = InColumn;

	CreateGrid();
}

void FYJBlockGrid::SetColor(const TArray<FIntPoint>& Positions, const FColor& NewColor)
{
	TArray<TSharedRef<FYJBlock>> RefreshedBlocks;
	RefreshedBlocks.Reserve(Positions.Num());

	for (const auto& Position : Positions)
	{
		auto OptionalBlock = GetBlock(Position);
		if (OptionalBlock.IsSet())
		{
			auto Block = OptionalBlock.GetValue();
			Block->SetColor(NewColor);
			RefreshedBlocks.Add(Block);
		}
	}

	OnBlockRefreshed().Broadcast(RefreshedBlocks);
}

void FYJBlockGrid::Refresh(const TArray<FIntPoint>& NonedPositions, const TArray<FIntPoint>& NewPositions, const FColor& NewColor)
{
	SetColor(NonedPositions, FYJBlockStatics::GetBlockNoneColor());
	SetColor(NewPositions, NewColor);
}

void FYJBlockGrid::BroadcastAllBlockRefreshed() const
{
	TArray<TSharedRef<FYJBlock>> RefreshedBlocks;
	RefreshedBlocks.Reserve(Row * Column);

	for (const auto& Blocks : Block2DArray)
	{
		RefreshedBlocks.Append(Blocks);
	}

	OnBlockRefreshed().Broadcast(RefreshedBlocks);
}

bool FYJBlockGrid::IsGridOut(const FIntPoint& Position) const
{
	if (Position.X < 0 || Position.X >= Column || Position.Y < 0 || Position.Y >= Row)
	{
		UE_LOG(LogYJ, Log, TEXT("%s is grid out !, Row %d Column %d"), *Position.ToString(), Row, Column);
		return true;
	}

	return false;
}

TOptional<TSharedRef<const FYJBlock>> FYJBlockGrid::GetBlock(const FIntPoint& Position) const
{
	if (IsGridOut(Position))
	{
		return TOptional<TSharedRef<const FYJBlock>>();
	}
	return Block2DArray[Position.Y][Position.X];
}

TOptional<TSharedRef<FYJBlock>> FYJBlockGrid::GetBlock(const FIntPoint& Position)
{
	if (IsGridOut(Position))
	{
		return TOptional<TSharedRef<FYJBlock>>();
	}
	return Block2DArray[Position.Y][Position.X];
}

bool FYJBlockGrid::CheckBlockCanMoveTo(const TArray<FIntPoint>& CurrBlockPositions, const TArray<FIntPoint>& BlockPositionsToMove) const
{
	for (const auto& ToMoveBlockPosition : BlockPositionsToMove)
	{
		bool bCanMove = false;
		bCanMove |= INDEX_NONE != CurrBlockPositions.Find(ToMoveBlockPosition); // 이동할 위치가 현재 위치면 가능
		bCanMove |= IsNoneBlockPosition(ToMoveBlockPosition); // 이동할 위치가 논 블록이면 가능

		if (!bCanMove)
		{
			UE_LOG(LogYJ, Log, TEXT("Can't Move !"));
			return false;
		}
	}

	return true;
}

bool FYJBlockGrid::IsNoneBlockPosition(const FIntPoint& Position) const
{
	const auto Block = GetBlock(Position);
	if (!Block.IsSet())
	{
		return false;
	}
	if (FYJBlockStatics::GetBlockNoneColor() == Block.GetValue()->GetColor())
	{
		return true;
	}

	return false;
}

bool FYJBlockGrid::IsBlockExist(const FYJTetrisBlock& TetrisBlock) const
{
	for (const FIntPoint& BlockPosition : TetrisBlock.GetBlockPositions())
	{
		const auto Block = GetBlock(BlockPosition);
		if (!Block.IsSet())
		{
			continue;
		}
		if (FYJBlockStatics::GetBlockNoneColor() != Block.GetValue()->GetColor())
		{
			return true;
		}
	}

	return false;
}

void FYJBlockGrid::ClearAllBlocks()
{
	for (auto& RowBlock : Block2DArray)
	{
		for (auto& ColumnBlock : RowBlock)
		{
			ColumnBlock->SetColor(FYJBlockStatics::GetBlockNoneColor());
		}
	}

	BroadcastAllBlockRefreshed();
}

bool FYJBlockGrid::RequestMove(FYJTetrisBlock& TetrisBlock, EYJMoveDirection MoveDirection)
{
	return MoveInternal(TetrisBlock, MoveDirection, true);
}

bool FYJBlockGrid::RequestDrop(FYJTetrisBlock& TetrisBlock)
{
	return MoveInternal(TetrisBlock, EYJMoveDirection::Down, false);
}

bool FYJBlockGrid::MoveInternal(FYJTetrisBlock& TetrisBlock, EYJMoveDirection MoveDirection, bool bOnceMove)
{
	const TArray<FIntPoint>& CurrBlockPositions = TetrisBlock.GetBlockPositions();
	TArray<FIntPoint> BlockPositionsToMove = CurrBlockPositions;

	auto AddBlockPositionsToMove = [&BlockPositionsToMove](const FIntPoint& MoveValue)
	{
		for (auto& BlockPositionToMove : BlockPositionsToMove)
		{
			BlockPositionToMove += MoveValue;
		}
	};

	int32 MoveCount = 0;

	do
	{
		AddBlockPositionsToMove(FYJBlockStatics::MoveDirectionMap[MoveDirection]);
		
		if (!CheckBlockCanMoveTo(CurrBlockPositions, BlockPositionsToMove))
		{
			AddBlockPositionsToMove(FYJBlockStatics::MoveDirectionMap[MoveDirection] * -1);
			break;
		}
		else
		{
			TetrisBlock.Move(MoveDirection);
			MoveCount++;
		}

	} while (!bOnceMove);

	if (MoveCount > 0)
	{
		Refresh(CurrBlockPositions, BlockPositionsToMove, TetrisBlock.GetColor());
	}

	return MoveCount > 0;
}

bool FYJBlockGrid::RequestRotate(FYJTetrisBlock& TetrisBlock, bool bClockWise)
{
	const TArray<FIntPoint> CurrBlockPositions = TetrisBlock.GetBlockPositions(); 
	const auto& Rotation = GetRotation90Degree(bClockWise);
	
	TetrisBlock.Rotate(Rotation);
	const auto& RotatedBlockPositions = TetrisBlock.GetBlockPositions();

	if (CheckBlockCanMoveTo(CurrBlockPositions, RotatedBlockPositions))
	{
		Refresh(CurrBlockPositions, RotatedBlockPositions, TetrisBlock.GetColor());
		return true;
	}
	else
	{
		TetrisBlock.Rotate(Rotation.Inverse());
		return false;
	}
}

bool FYJBlockGrid::RequestDeleteLine()
{
	// 1. 밑에서 위로 꽉찬 줄을 찾음 (찾았다면 한칸씩 내림)
	// 2. 마지막 줄까지 반복
	// 3. 전체 블록 갱신.
	
	bool bDeletedLine = false;

	for (int32 RowIndex = 0; RowIndex != Block2DArray.Num(); RowIndex++)
	{
		auto& Blocks = Block2DArray[RowIndex];

		bool bCanDelete = true;
		for (int32 ColumnIndex = 0; ColumnIndex < Blocks.Num(); ColumnIndex++)
		{
			if (Blocks[ColumnIndex]->GetColor() == FYJBlockStatics::GetBlockNoneColor())
			{
				bCanDelete = false;
				break;
			}
		}

		if (bCanDelete)
		{
			bDeletedLine = true;
			if (RowIndex == 0)
			{
				for (auto& Block : Blocks)
				{
					Block->SetColor(FYJBlockStatics::GetBlockNoneColor());
				}
			}

			for (int32 RowIndex2 = RowIndex; RowIndex2 > 0; RowIndex2--)
			{
				auto& CurrRowBlocks = Block2DArray[RowIndex2];
				auto& UpperRowBlocks = Block2DArray[RowIndex2 - 1];
				for (int32 ColumnIndex = 0; ColumnIndex < CurrRowBlocks.Num(); ColumnIndex++)
				{
					CurrRowBlocks[ColumnIndex]->SetColor(UpperRowBlocks[ColumnIndex]->GetColor());
				}
			}
		}
	}
	
	if (bDeletedLine)
	{
		BroadcastAllBlockRefreshed();
	}

	return bDeletedLine;
}

const FQuat2D& FYJBlockGrid::GetRotation90Degree(bool bClockWise)
{
	static const FQuat2D RotationClockWise = FQuat2D(FMath::DegreesToRadians(90.0f));
	static const FQuat2D RotationInverseClockWise = RotationClockWise.Inverse();

	return bClockWise ? RotationClockWise : RotationInverseClockWise;
}

void FYJBlockGrid::CreateGrid()
{
	Block2DArray.Empty();

	Block2DArray.SetNum(Row);
	for (int32 RowIndex = 0; RowIndex < Row; RowIndex++)
	{
		Block2DArray[RowIndex].Reserve(Column);
		for (int32 ColumnIndex = 0; ColumnIndex < Column; ColumnIndex++)
		{
			Block2DArray[RowIndex].Add(MakeShared<FYJBlock>(FIntPoint(ColumnIndex, RowIndex), FYJBlockStatics::GetBlockNoneColor()));
		}
	}
}


