// Copyright 2017-2018 Csaba Molnar, Daniel Butum
#pragma once
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SCompoundWidget.h"
#include "STreeView.h"
#include "Widgets/Input/SSearchBox.h"

#include "DlgDataDisplayTreeNode.h"
#include "DlgDataDisplayActorProperties.h"

class SDlgDataPropertyValue : public SCompoundWidget
{
	typedef SDlgDataPropertyValue Self;
	typedef SCompoundWidget Super;
public:
	SLATE_BEGIN_ARGS(Self) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedPtr<FDlgDataDisplayTreeVariableNode> InVariableNode);

	// SWidget Interface
	/**
	 * Ticks this widget with Geometry.  Override in derived classes, but always call the parent implementation.
	 *
	 * @param  AllottedGeometry The space allotted for this widget
	 * @param  InCurrentTime  Current absolute real time
	 * @param  InDeltaTime  Real time passed since last tick
	 */
	void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	/** Gets the Value of this Property as an FText; */
	FText GetTextValue() const { return FText::FromString(VariableNode->GetVariableValue()); }

protected:
	/** Updates the VariableNode value from the Actor. */
	void UpdateVariableNodeFromActor();

protected:
	/** The Node this widget value represents */
	TSharedPtr<FDlgDataDisplayTreeVariableNode> VariableNode;

	/** Number of seconds passed in the Tick */
	float TickPassedDeltaTimeSeconds = 0.f;

	static constexpr float TickUpdateTimeSeconds = 1.0f;
};


class SDlgDataTextPropertyValue : public SDlgDataPropertyValue
{
	typedef SDlgDataTextPropertyValue Self;
	typedef SDlgDataPropertyValue Super;
public:
	SLATE_BEGIN_ARGS(Self) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedPtr<FDlgDataDisplayTreeVariableNode> InVariableNode);

protected:
	void HandleTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo);
	void HandleTextChanged(const FText& NewText);

	bool IsReadOnly() const { return !VariableNode.IsValid(); }

protected:
	/** Widget used for the single line version of the text property */
	TSharedPtr<SEditableTextBox> TextBoxWidget;

	/** True if property is an FName property which causes us to run extra size validation checks */
	bool bIsFNameProperty = false;
};


/**
 * Implements the Runtime Dialogue Data Display
 */
class SDlgDataDisplay : public SCompoundWidget
{
	typedef SDlgDataDisplay Self;

public:
	SLATE_BEGIN_ARGS(Self) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TWeakObjectPtr<AActor> InReferenceActor = nullptr);

	/** Updates the actors tree. */
	void RefreshTree(bool bPreserveExpansion);

	/** Get current filter text */
	FText GetFilterText() const { return FilterTextBoxWidget->GetText(); }

private:
	/** Getters for widgets.  */
	TSharedRef<SWidget> GetFilterTextBoxWidget();

	/** Recursively build the view item. */
	void BuildTreeViewItem(FDlgDataDisplayTreeNodePtr Item);

	/** Text search changed */
	void HandleSearchTextCommited(const FText& InText, ETextCommit::Type InCommitType);

	/** Refresh button clicked. */
	FReply HandleOnRefresh()
	{
		RefreshTree(true);
		return FReply::Handled();
	}

	/** Make the row */
	TSharedRef<ITableRow> HandleGenerateRow(FDlgDataDisplayTreeNodePtr InItem, const TSharedRef<STableViewBase>& OwnerTable);

	/** General Get children  */
	void HandleGetChildren(FDlgDataDisplayTreeNodePtr InItem, TArray<FDlgDataDisplayTreeNodePtr>& OutChildren);

	/** Handles changes in the Tree View. */
	void HandleTreeSelectionChanged(FDlgDataDisplayTreeNodePtr InItem, ESelectInfo::Type SelectInfo);

	/** User clicked on item. */
	void HandleDoubleClick(FDlgDataDisplayTreeNodePtr InItem);

	/** Callback for expanding tree items recursively */
	void HandleSetExpansionRecursive(FDlgDataDisplayTreeNodePtr InItem, bool bInIsItemExpanded);

private:
	/** The search box */
	TSharedPtr<SSearchBox> FilterTextBoxWidget;

	/** The filter text from the search box. */
	FString FilterString;

	/** The root data source */
	FDlgDataDisplayTreeNodePtr RootTreeItem;

	/** The root children. Kept seperate so that we do not corrupt the data. */
	TArray<FDlgDataDisplayTreeNodePtr> RootChildren;

	/** Tree view for showing all Actors that implement the , etc. */
	TSharedPtr<STreeView<FDlgDataDisplayTreeNodePtr>> ActorsTreeView;

	/**
	 * Used for fast lookup of each actor
	 * Key: Weak Pointer to the Actor
	 * Value: Actor properties
	 */
	TMap<TWeakObjectPtr<const AActor>, TSharedPtr<FDlgDataDisplayActorProperties>> ActorsProperties;

	/** Reference Actor used to get the UWorld. */
	TWeakObjectPtr<const AActor> ReferenceActor = nullptr;
};
