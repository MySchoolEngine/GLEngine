#include "stdafx.h"

#include "QWidgetRendererBase.h"
#include <QMainWindow>
#include <QOpenGLContext>
#include <QDebug>

//=================================================================================
C_QWidgetRendererBase::C_QWidgetRendererBase(QWidget * parent)
	: QOpenGLWidget(parent)
	, m_Init(false)
	, m_Updates(false)
	, m_fpsLimit(30)
	, m_Timer(this)
{
}

//=================================================================================
void C_QWidgetRendererBase::RunUpdates(bool run /*= true*/)
{
	connect(&m_Timer, SIGNAL(timeout()), this, SLOT(update()));
	m_Timer.start(0);
}

//=================================================================================
void C_QWidgetRendererBase::paintGL()
{
	this->makeCurrent();
	this->beforeRendering();
	this->draw();
	this->doneCurrent();

}

//=================================================================================
void C_QWidgetRendererBase::initializeGL()
{
	makeCurrent();
	this->m_Init = true;
}
