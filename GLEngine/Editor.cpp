#include "stdafx.h"

#include "Editor.h"
#include "GLWidget.h"

#include <QMenuBar>
#include <QMenu>

//=================================================================================
C_Editor::C_Editor(QWidget *parent)
{
	m_glWidget = new C_GLWidget(this);
	this->setCentralWidget(m_glWidget);

	setWindowTitle(tr("Hello GL"));
	setMinimumSize(400, 400);
}