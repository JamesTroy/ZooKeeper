#include "AnimalInfoWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "Animals/AnimalBase.h"
#include "Animals/AnimalNeedsComponent.h"
#include "ZooKeeper.h"

TSharedRef<SWidget> UAnimalInfoWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		BuildWidgetTree();
	}
	return Super::RebuildWidget();
}

void UAnimalInfoWidget::BuildWidgetTree()
{
	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
	WidgetTree->RootWidget = RootCanvas;

	// -- Semi-transparent background on right side --
	UImage* Background = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Background"));
	Background->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.6f));
	UCanvasPanelSlot* BgSlot = RootCanvas->AddChildToCanvas(Background);
	BgSlot->SetAnchors(FAnchors(1.0f, 0.0f, 1.0f, 1.0f));
	BgSlot->SetAlignment(FVector2D(1.0f, 0.0f));
	BgSlot->SetOffsets(FMargin(320.0f, 0.0f, 0.0f, 0.0f)); // left offset = width

	// -- Content vertical box overlaid on the background --
	UVerticalBox* ContentBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ContentBox"));
	UCanvasPanelSlot* ContentSlot = RootCanvas->AddChildToCanvas(ContentBox);
	ContentSlot->SetAnchors(FAnchors(1.0f, 0.0f, 1.0f, 1.0f));
	ContentSlot->SetAlignment(FVector2D(1.0f, 0.0f));
	ContentSlot->SetOffsets(FMargin(300.0f, 10.0f, 10.0f, 10.0f));

	// -- Animal Name (22pt) --
	AnimalNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("AnimalNameText"));
	AnimalNameText->SetText(FText::GetEmpty());
	FSlateFontInfo NameFont = AnimalNameText->GetFont();
	NameFont.Size = 22;
	AnimalNameText->SetFont(NameFont);
	AnimalNameText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	ContentBox->AddChildToVerticalBox(AnimalNameText);

	// -- Species (16pt) --
	SpeciesText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SpeciesText"));
	SpeciesText->SetText(FText::GetEmpty());
	FSlateFontInfo SpeciesFont = SpeciesText->GetFont();
	SpeciesFont.Size = 16;
	SpeciesText->SetFont(SpeciesFont);
	SpeciesText->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
	ContentBox->AddChildToVerticalBox(SpeciesText);

	// -- Age (14pt) --
	AgeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("AgeText"));
	AgeText->SetText(FText::GetEmpty());
	FSlateFontInfo AgeFont = AgeText->GetFont();
	AgeFont.Size = 14;
	AgeText->SetFont(AgeFont);
	AgeText->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)));
	ContentBox->AddChildToVerticalBox(AgeText);

	// -- Spacer (10px) --
	USpacer* Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass(), TEXT("NeedsSpacer"));
	Spacer->SetSize(FVector2D(0.0f, 10.0f));
	ContentBox->AddChildToVerticalBox(Spacer);

	// -- Need bars --
	HungerBar    = CreateNeedRow(ContentBox, TEXT("Hunger"),    FLinearColor(0.2f, 0.8f, 0.2f)); // green
	ThirstBar    = CreateNeedRow(ContentBox, TEXT("Thirst"),    FLinearColor(0.2f, 0.5f, 1.0f)); // blue
	EnergyBar    = CreateNeedRow(ContentBox, TEXT("Energy"),    FLinearColor(1.0f, 0.9f, 0.2f)); // yellow
	HealthBar    = CreateNeedRow(ContentBox, TEXT("Health"),    FLinearColor(0.9f, 0.2f, 0.2f)); // red
	HappinessBar = CreateNeedRow(ContentBox, TEXT("Happiness"), FLinearColor(1.0f, 0.5f, 0.7f)); // pink
	SocialBar    = CreateNeedRow(ContentBox, TEXT("Social"),    FLinearColor(0.2f, 0.9f, 0.9f)); // cyan
}

UProgressBar* UAnimalInfoWidget::CreateNeedRow(UVerticalBox* Parent, const FString& LabelStr, const FLinearColor& BarColor)
{
	UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(),
		*FString::Printf(TEXT("%sRow"), *LabelStr));

	// Label
	UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
		*FString::Printf(TEXT("%sLabel"), *LabelStr));
	Label->SetText(FText::FromString(LabelStr));
	FSlateFontInfo LabelFont = Label->GetFont();
	LabelFont.Size = 12;
	Label->SetFont(LabelFont);
	Label->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	UHorizontalBoxSlot* LabelSlot = Row->AddChildToHorizontalBox(Label);
	LabelSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	LabelSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);

	// Progress bar
	UProgressBar* Bar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(),
		*FString::Printf(TEXT("%sBar"), *LabelStr));
	Bar->SetPercent(0.0f);
	Bar->SetFillColorAndOpacity(BarColor);
	UHorizontalBoxSlot* BarSlot = Row->AddChildToHorizontalBox(Bar);
	BarSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	BarSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);

	UVerticalBoxSlot* RowSlot = Parent->AddChildToVerticalBox(Row);
	RowSlot->SetPadding(FMargin(0.0f, 2.0f));

	return Bar;
}

void UAnimalInfoWidget::SetAnimalData(AAnimalBase* Animal)
{
	if (!Animal)
	{
		ClearAnimalData();
		return;
	}

	// Unbind from previous animal's delegate if any.
	if (CurrentAnimal.IsValid())
	{
		if (UAnimalNeedsComponent* OldNeeds = CurrentAnimal->NeedsComponent)
		{
			OldNeeds->OnNeedChanged.RemoveDynamic(this, &UAnimalInfoWidget::HandleNeedChanged);
		}
	}

	CurrentAnimal = Animal;

	// Bind to new animal's OnNeedChanged delegate for live updates.
	if (UAnimalNeedsComponent* Needs = Animal->NeedsComponent)
	{
		Needs->OnNeedChanged.AddDynamic(this, &UAnimalInfoWidget::HandleNeedChanged);
	}

	if (AnimalNameText)
	{
		AnimalNameText->SetText(FText::FromString(Animal->AnimalName));
	}

	if (SpeciesText)
	{
		SpeciesText->SetText(FText::FromName(Animal->SpeciesID));
	}

	if (AgeText)
	{
		AgeText->SetText(FText::FromString(FString::Printf(TEXT("%d days"), Animal->Age)));
	}

	UpdateNeedBars();

	UE_LOG(LogZooKeeper, Log, TEXT("AnimalInfoWidget: Displaying data for '%s' (%s)."),
		*Animal->AnimalName, *Animal->SpeciesID.ToString());
}

void UAnimalInfoWidget::UpdateNeedBars()
{
	if (!CurrentAnimal.IsValid())
	{
		return;
	}

	AAnimalBase* Animal = CurrentAnimal.Get();
	UAnimalNeedsComponent* Needs = Animal->NeedsComponent;
	if (!Needs)
	{
		UE_LOG(LogZooKeeper, Warning, TEXT("AnimalInfoWidget: Animal '%s' has no NeedsComponent."),
			*Animal->AnimalName);
		return;
	}

	if (HungerBar)    { HungerBar->SetPercent(Needs->Hunger); }
	if (ThirstBar)    { ThirstBar->SetPercent(Needs->Thirst); }
	if (EnergyBar)    { EnergyBar->SetPercent(Needs->Energy); }
	if (HealthBar)    { HealthBar->SetPercent(Needs->Health); }
	if (HappinessBar) { HappinessBar->SetPercent(Needs->Happiness); }
	if (SocialBar)    { SocialBar->SetPercent(Needs->Social); }
}

void UAnimalInfoWidget::ClearAnimalData()
{
	// Unbind from the previous animal's delegate.
	if (CurrentAnimal.IsValid())
	{
		if (UAnimalNeedsComponent* Needs = CurrentAnimal->NeedsComponent)
		{
			Needs->OnNeedChanged.RemoveDynamic(this, &UAnimalInfoWidget::HandleNeedChanged);
		}
	}

	CurrentAnimal.Reset();

	if (AnimalNameText) { AnimalNameText->SetText(FText::GetEmpty()); }
	if (SpeciesText)    { SpeciesText->SetText(FText::GetEmpty()); }
	if (AgeText)        { AgeText->SetText(FText::GetEmpty()); }
	if (HungerBar)      { HungerBar->SetPercent(0.0f); }
	if (ThirstBar)      { ThirstBar->SetPercent(0.0f); }
	if (EnergyBar)      { EnergyBar->SetPercent(0.0f); }
	if (HealthBar)      { HealthBar->SetPercent(0.0f); }
	if (HappinessBar)   { HappinessBar->SetPercent(0.0f); }
	if (SocialBar)      { SocialBar->SetPercent(0.0f); }
}

void UAnimalInfoWidget::HandleNeedChanged(FName NeedName, float NewValue)
{
	if (!CurrentAnimal.IsValid())
	{
		return;
	}

	// Update the specific bar that changed rather than all bars.
	if (NeedName == FName("Hunger") && HungerBar)       { HungerBar->SetPercent(NewValue); }
	else if (NeedName == FName("Thirst") && ThirstBar)   { ThirstBar->SetPercent(NewValue); }
	else if (NeedName == FName("Energy") && EnergyBar)   { EnergyBar->SetPercent(NewValue); }
	else if (NeedName == FName("Health") && HealthBar)   { HealthBar->SetPercent(NewValue); }
	else if (NeedName == FName("Happiness") && HappinessBar) { HappinessBar->SetPercent(NewValue); }
	else if (NeedName == FName("Social") && SocialBar)   { SocialBar->SetPercent(NewValue); }
}
