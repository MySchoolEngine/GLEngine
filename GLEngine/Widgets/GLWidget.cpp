#include "stdafx.h"

#include "Widgets/GLWidget.h"

#include <algorithm>

//=================================================================================
C_GLWidget::C_GLWidget(QWidget *parent)
	: C_QWidgetRendererBase(parent)
	, m_red(0.0)
{

	QSizePolicy po;
	po.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
	po.setVerticalPolicy(QSizePolicy::MinimumExpanding);
	po.setVerticalStretch(1);
	po.setHorizontalStretch(1);

	this->setSizePolicy(po);
}

//=================================================================================
C_GLWidget::~C_GLWidget()
{
	Cleanup();
}

//=================================================================================
QSize C_GLWidget::sizeHint() const
{
	return QSize(400, 400);
}

//=================================================================================
void C_GLWidget::Cleanup()
{
	makeCurrent();
	//actual cleaning
	doneCurrent();
}

//=================================================================================
void C_GLWidget::initializeGL()
{
	C_QWidgetRendererBase::initializeGL();
	glClearColor(0, 0, 0, true ? 0 : 1);

	doneCurrent();

	RunUpdates();
}

//=================================================================================
void C_GLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(m_red, 0.0, 0.0, 0.0);

	m_red = (m_red + 0.05);
	if (m_red >= 1.0) {
		m_red -= 1.0f;
	}
}

//=================================================================================
void C_GLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	//m_proj.setToIdentity();
	//m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

//=================================================================================
void C_GLWidget::mousePressEvent(QMouseEvent *event)
{
	m_lastPos = event->pos();
}

//=================================================================================
void C_GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	//int dx = event->x() - m_lastPos.x();
	//int dy = event->y() - m_lastPos.y();
	//
	//if (event->buttons() & Qt::LeftButton) {
	//	//setXRotation(m_xRot + 8 * dy);
	//	//setYRotation(m_yRot + 8 * dx);
	//}
	//else if (event->buttons() & Qt::RightButton) {
	//	//setXRotation(m_xRot + 8 * dy);
	//	//setZRotation(m_zRot + 8 * dx);
	//}
	m_lastPos = event->pos();
}

//=================================================================================
void C_GLWidget::beforeRendering()
{
	qDebug() << "BeforeRender";
}

//=================================================================================
void C_GLWidget::draw()
{
	qDebug() << "draw";
}
