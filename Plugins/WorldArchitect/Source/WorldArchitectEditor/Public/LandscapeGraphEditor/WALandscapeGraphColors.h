#pragma once

namespace WALandscapeGraphColors
{
	namespace NodeBody
	{
		const FLinearColor Default(0.1f, 0.1f, 0.1f);
		const FLinearColor Root(0.5f, 0.5f, 0.5f, 0.1f);
		const FLinearColor Error(1.0f, 0.0f, 0.0f);
	}

	namespace NodeBorder
	{
		const FLinearColor Inactive(0.08f, 0.08f, 0.08f);
		const FLinearColor Root(0.2f, 0.2f, 0.2f, 0.2f);
		const FLinearColor Selected(1.00f, 0.08f, 0.08f);
		const FLinearColor ActiveDebugging(1.0f, 1.0f, 0.0f);
		const FLinearColor InactiveDebugging(0.4f, 0.4f, 0.0f);
		const FLinearColor HighlightAbortRange0(0.0f, 0.22f, 0.4f);
		const FLinearColor HighlightAbortRange1(0.0f, 0.4f, 0.22f);
		const FLinearColor Disconnected(0.f, 0.f, 0.f);
		const FLinearColor BrokenWithParent(1.f, 0.f, 1.f);
		const FLinearColor QuickFind(0.f, 0.8f, 0.f);
	}

	namespace Pin
	{
		const FLinearColor Diff(0.9f, 0.2f, 0.15f);
		const FLinearColor Hover(1.0f, 0.7f, 0.0f);
		const FLinearColor Default(0.02f, 0.02f, 0.02f);
		const FLinearColor SingleNode(0.02f, 0.02f, 0.02f);
	}
	
	namespace Connection
	{
		const FLinearColor Default(1.0f, 1.0f, 1.0f);
	}

	namespace Action
	{
		const FLinearColor DragMarker(1.0f, 1.0f, 0.2f);
	}
}
