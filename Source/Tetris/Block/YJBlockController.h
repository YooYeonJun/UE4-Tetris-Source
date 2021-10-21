#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "YJBlockController.generated.h"

class FYJBlockGrid;
class UYJBlockGridWidget;
class FYJTetrisBlock;
struct FYJBlock;

enum class EYJMoveDirection : uint8;

/**
 * 블록 컨트롤러
 */
UCLASS()
class TETRIS_API UYJBlockController : public UObject
{
	GENERATED_BODY()

public:
	UYJBlockController();
	UYJBlockController(FVTableHelper& Helper);

	virtual ~UYJBlockController();

	void Init(int32 InRow, int32 Inlumn, UYJBlockGridWidget* InBlockGridWidget);

public: // Test
	void ChangeCurrentTetrisBlock_Test() { CreateNewTetrisBlock(); }

public: // Input Control
	bool MovePressed(EYJMoveDirection MoveDirection);
	void CancelMovePressed(EYJMoveDirection MoveDirection);

	bool Rotate(bool bClockWise = true) const;
	bool Drop();

private: // Create Block
	TSharedPtr<FYJTetrisBlock> CreateRandomTetrisBlock();
	void CreateTetrisBlockQueue();
	void EnqueueTetrisBlock();

	void CreateNewTetrisBlock();
	void OnCantCreateNewBlock() const;

	void OnRefreshedBlock(const TArray<FYJBlock>& RefreshedBlocks);

private: // Move Block
	UFUNCTION()
	void OnMoveDown();
	void MoveDown();
	void SetTimer_OnMoveDown();

	UFUNCTION()
	void OnMovePressed();
	void SetTimer_OnMovePressed();
	void ClearTimerHandle_OnMovePressed();

	bool Move(EYJMoveDirection MoveDirection) const;

private:
	bool IsValid() const;

private:
	TUniquePtr<FYJBlockGrid> BlockGrid;
	TWeakObjectPtr<UYJBlockGridWidget> BlockGridWidget;

	TSharedPtr<FYJTetrisBlock> CurrTetrisBlock;
	TQueue<TSharedPtr<FYJTetrisBlock>> TetrisBlockQueue;

	FTimerHandle TimerHandle_MoveDown;
	float MoveDownTime = 1.0f;

	FTimerHandle TimerHandle_MovePressed;
	float MovePressedTime = 0.1f;
	TSet<EYJMoveDirection> PressedMoveDirectionSet;

};
