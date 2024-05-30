#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Misc/NotifyHook.h"
#include "GraphEditor.h"
#include "IDetailsView.h"

class FDungeonRulesToolkit : public FAssetEditorToolkit, public FEditorUndoClient, public FNotifyHook
{
public:
	FDungeonRulesToolkit();
	~FDungeonRulesToolkit();

	//~ BEGIN FAssetEditorToolkit
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void SaveAsset_Execute() override;
	//~ END FAssetEditorToolkit

	virtual void InitGraphAssetEditor(const EToolkitMode::Type InMode, const TSharedPtr<class IToolkitHost>& InToolkitHost, UObject* ObjectToEdit);
	//virtual void BlueprintCompiled();


	FGraphPanelSelectionSet GetSelectedNodes() const;
	virtual void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);

	//~ Begin FEditorUndoClient Interface
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	// End of FEditorUndoClient

	void CreateCommandList();
	
	// Delegates for graph editor commands
	void SelectAllNodes();
	bool CanSelectAllNodes() const;
	void DeleteSelectedNodes();
	bool CanDeleteNodes() const;
	void DeleteSelectedDuplicatableNodes();
	void CutSelectedNodes();
	bool CanCutNodes() const;
	void CopySelectedNodes();
	bool CanCopyNodes() const;
	void PasteNodes();
	void PasteNodesHere(const FVector2D& Location);
	bool CanPasteNodes() const;
	void DuplicateNodes();
	bool CanDuplicateNodes() const;

	bool CanCreateComment() const;
	void OnCreateComment();

/*
	virtual void OnClassListUpdated();
	*/

protected:
	//virtual void FixupPastedNodes(const TSet<UEdGraphNode*>& NewPastedGraphNodes, const TMap<FGuid/*New*/, FGuid/*Old*/>& NewToOldNodeMapping);

protected:

	/** Currently focused graph */
	TWeakPtr<SGraphEditor> UpdateGraphEdPtr;

	/** The command list for this editor */
	TSharedPtr<FUICommandList> GraphEditorCommands;

	/** Handle to the registered OnClassListUpdated delegate */
	FDelegateHandle OnClassListUpdatedDelegateHandle;

private:
	static const FName DetailsTabId;
	static const FName GraphTabId;

	class UDungeonRules* DungeonRules;
	TSharedPtr<SGraphEditor> EdGraphEditor;
	TSharedPtr<IDetailsView> DetailsWidget;

	TSharedRef<SDockTab> SpawnTab_Properties(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_GraphCanvas(const FSpawnTabArgs& Args);

	/** Creates all internal widgets for the tabs to point at */
	void CreateInternalWidgets();

	/** Create new graph editor widget */
	TSharedRef<SGraphEditor> CreateGraphEditorWidget();
};
