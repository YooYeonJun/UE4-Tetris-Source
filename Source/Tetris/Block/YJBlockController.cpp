#include "YJBlockController.h"
#include "../Tetris.h"
#include "YJBlock.h"
#include "YJBlockWidget.h"

UYJBlockController::UYJBlockController()
{

}

UYJBlockController::UYJBlockController(FVTableHelper& Helper)
	:Super(Helper)
{
}

UYJBlockController::~UYJBlockController()
{
}

void UYJBlockController::Init(int32 InRow, int32 InColumn, UYJBlockGridWidget* InBlockGridWidget)
{
	check(InBlockGridWidget);

	BlockGrid = MakeUnique<FYJBlockGrid>();
	BlockGrid->OnBlockRefreshed().AddUObject(this, &UYJBlockController::OnRefreshedBlock);
	BlockGrid->Init(InRow, InColumn);

	BlockGridWidget = MakeWeakObjectPtr<UYJBlockGridWidget>(InBlockGridWidget);
	BlockGridWidget->Init(InRow, InColumn, FLinearColor(FYJBlockStatics::GetBlockNoneColor()));

	CreateTetrisBlockQueue();
}

bool UYJBlockController::MovePressed(EYJMoveDirection MoveDirection)
{
	const bool Result = Move(MoveDirection);

	PressedMoveDirectionSet.Add(MoveDirection);
	
	SetTimer_OnMovePressed();

	return Result;
}

void UYJBlockController::CancelMovePressed(EYJMoveDirection MoveDirection)
{
	PressedMoveDirectionSet.Remove(MoveDirection);

	if (PressedMoveDirectionSet.Num() == 0)
	{
		ClearTimerHandle_OnMovePressed();
	}
}

bool UYJBlockController::Rotate(bool bClockWise) const
{
	if (!IsValid())
	{
		return false;
	}

	return BlockGrid->RequestRotate(*CurrTetrisBlock.Get(), bClockWise);
}

bool UYJBlockController::Drop()
{
	if (!IsValid())
	{
		return false;
	}

	const bool bCanDrop = BlockGrid->RequestDrop(*CurrTetrisBlock.Get());
	CreateNewTetrisBlock();

	return bCanDrop;
}

TSharedPtr<FYJTetrisBlock> UYJBlockController::CreateRandomTetrisBlock()
{
	EYJBlockType BlockType = static_cast<EYJBlockType>(FMath::Rand() % static_cast<int32>(EYJBlockType::Num));
	TSharedPtr<FYJTetrisBlock> TetrisBlock = MakeShared<FYJTetrisBlock>(BlockType, FIntPoint(0, 0));
	
	return TetrisBlock;
}

void UYJBlockController::CreateTetrisBlockQueue()
{
	static const int32 QueueSize = 3;

	for (int32 i = 0; i < QueueSize; i++)
	{
		EnqueueTetrisBlock();
	}

	CreateNewTetrisBlock();
}

void UYJBlockController::EnqueueTetrisBlock()
{
	TetrisBlockQueue.Enqueue(CreateRandomTetrisBlock());
}

void UYJBlockController::OnCantCreateNewBlock() const
{
	BlockGrid->ClearAllBlocks();
}

void UYJBlockController::CreateNewTetrisBlock()
{
	SetTimer_OnMoveDown();

	if (!TetrisBlockQueue.Dequeue(CurrTetrisBlock))
	{
		UE_LOG(LogYJ, Warning, TEXT("TetrisQueue is empty !"));
		return;
	}
	CurrTetrisBlock->SetPosition(BlockGrid->GetBlockStartPosition());

	if (BlockGrid->IsBlockExist(*CurrTetrisBlock))
	{
		OnCantCreateNewBlock();
	}

	EnqueueTetrisBlock();

	MoveDown();
}

void UYJBlockController::OnRefreshedBlock(const TArray<FYJBlock>& RefreshedBlocks)
{
	UE_LOG(LogYJ, Log, TEXT(__FUNCTION__));

	for (const auto& RefreshedBlock : RefreshedBlocks)
	{
		BlockGridWidget->Refresh(RefreshedBlock.Position.Y, RefreshedBlock.Position.X, RefreshedBlock.Color);
	}

	// SetBlockInfo
	if (CurrTetrisBlock)
	{
		FString BlockInfo;
		const auto& BlockPositions = CurrTetrisBlock->GetBlockPositions();
		for (int32 i = 0; i < BlockPositions.Num(); i++)
		{
			BlockInfo += FString::Printf(TEXT("BlockPosition_%d : "), i) + BlockPositions[i].ToString() + TEXT("\n");
		}

		BlockGridWidget->SetBlockInfo(FText::FromString(BlockInfo));
	}

	BlockGrid->RequestDeleteLine();
}

void UYJBlockController::OnMoveDown()
{
	if (!PressedMoveDirectionSet.Contains(EYJMoveDirection::Down))
	{
		MoveDown();
	}
}

void UYJBlockController::MoveDown()
{
	if (!Move(EYJMoveDirection::Down))
	{
		CreateNewTetrisBlock();
	}
}

void UYJBlockController::SetTimer_OnMoveDown()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MoveDown, this, &UYJBlockController::OnMoveDown, MoveDownTime, true);
}

void UYJBlockController::OnMovePressed()
{
	for (const auto& MoveDirection : PressedMoveDirectionSet)
	{
		Move(MoveDirection);
	}
}

void UYJBlockController::SetTimer_OnMovePressed()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MovePressed, this, &UYJBlockController::OnMovePressed, MovePressedTime, true);
}

void UYJBlockController::ClearTimerHandle_OnMovePressed()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_MovePressed);
}

bool UYJBlockController::Move(EYJMoveDirection MoveDirection) const
{
	if (!IsValid())
	{
		return false;
	}

	return BlockGrid->RequestMove(*CurrTetrisBlock.Get(), MoveDirection);
}

bool UYJBlockController::IsValid() const
{
	return CurrTetrisBlock.IsValid();
}
