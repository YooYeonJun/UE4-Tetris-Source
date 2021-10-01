#include "YJPlayerControllerBase.h"
#include "../Tetris.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "../Block/YJBlockController.h"
#include "../Block/YJBlock.h"
#include "../Window/YJSingleModeWindow.h"

void AYJPlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Test
	InputComponent->BindAction(TEXT("ChangeBlock"), EInputEvent::IE_Pressed, this, &AYJPlayerControllerBase::OnChangeBlock);
	// ~ Test

	InputComponent->BindAction(TEXT("RotateClockWise"), EInputEvent::IE_Pressed, this, &AYJPlayerControllerBase::OnRotateClockWise);
	InputComponent->BindAction(TEXT("RotateCounterClockWise"), EInputEvent::IE_Pressed, this, &AYJPlayerControllerBase::OnRotateCounterClockWise);
	InputComponent->BindAction(TEXT("Drop"), EInputEvent::IE_Pressed, this, &AYJPlayerControllerBase::OnDrop);

	InputComponent->BindAction(TEXT("MoveLeft"), EInputEvent::IE_Pressed, this, &AYJPlayerControllerBase::OnMoveLeft_Pressed);
	InputComponent->BindAction(TEXT("MoveRight"), EInputEvent::IE_Pressed, this, &AYJPlayerControllerBase::OnMoveRight_Pressed);
	InputComponent->BindAction(TEXT("MoveDown"), EInputEvent::IE_Pressed, this, &AYJPlayerControllerBase::OnMoveDown_Pressed);

	InputComponent->BindAction(TEXT("MoveLeft"), EInputEvent::IE_Released, this, &AYJPlayerControllerBase::OnMoveLeft_Released);
	InputComponent->BindAction(TEXT("MoveRight"), EInputEvent::IE_Released, this, &AYJPlayerControllerBase::OnMoveRight_Released);
	InputComponent->BindAction(TEXT("MoveDown"), EInputEvent::IE_Released, this, &AYJPlayerControllerBase::OnMoveDown_Released);
}

void AYJPlayerControllerBase::OnChangeBlock()
{
	YJ_LOG_FUNCTION();
	BlockController->ChangeCurrentTetrisBlock_Test();
}

void AYJPlayerControllerBase::OnMoveLeft_Pressed(FKey Key)
{
	YJ_LOG_FUNCTION();
	BlockController->MovePressed(EYJMoveDirection::Left);
}

void AYJPlayerControllerBase::OnMoveRight_Pressed(FKey Key)
{
	YJ_LOG_FUNCTION();
	BlockController->MovePressed(EYJMoveDirection::Right);
}

void AYJPlayerControllerBase::OnMoveDown_Pressed(FKey Key)
{
	YJ_LOG_FUNCTION();
	BlockController->MovePressed(EYJMoveDirection::Down);
}

void AYJPlayerControllerBase::OnMoveLeft_Released(FKey Key)
{
	YJ_LOG_FUNCTION();
	BlockController->CancelMovePressed(EYJMoveDirection::Left);
}

void AYJPlayerControllerBase::OnMoveRight_Released(FKey Key)
{
	YJ_LOG_FUNCTION();
	BlockController->CancelMovePressed(EYJMoveDirection::Right);
}

void AYJPlayerControllerBase::OnMoveDown_Released(FKey Key)
{
	YJ_LOG_FUNCTION();
	BlockController->CancelMovePressed(EYJMoveDirection::Down);
}

void AYJPlayerControllerBase::OnRotateClockWise(FKey Key)
{
	YJ_LOG_FUNCTION();
	BlockController->Rotate();
}

void AYJPlayerControllerBase::OnRotateCounterClockWise(FKey Key)
{
	YJ_LOG_FUNCTION();
	BlockController->Rotate(false);
}

void AYJPlayerControllerBase::OnDrop(FKey Key)
{
	YJ_LOG_FUNCTION();
	BlockController->Drop();
}

void AYJPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (SingleModeWindowClass)
	{
		SingleModeWindow = CreateWidget<UYJSingleModeWindow>(this, SingleModeWindowClass, TEXT("SingleModeWindow"));
		SingleModeWindow->AddToViewport();

		UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);

		bShowMouseCursor = true;
	}

	BlockController = NewObject<UYJBlockController>(this);
	BlockController->Init(Row, Column, SingleModeWindow->GetBlockGridWidget());
}
