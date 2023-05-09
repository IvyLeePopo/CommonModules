///头文件

#include <QObject>
#include <QDialog>

class QRect;
class QLabel;
class SchemeItemTipsWidget : public QDialog
{
	Q_OBJECT
public:
	SchemeItemTipsWidget(QWidget *parent = nullptr);
	~SchemeItemTipsWidget();

public:
	virtual void show();
	void clear();
	bool containsData();
	void setName(const QString &name);
	void setComponents(const QString &componentName);
	void setParentGrometry(const QRect &rc);
	void setShowIntervel(int intervel);

protected:
	virtual void paintEvent(QPaintEvent *event) override;

private:
	void adjustWidth();
	void adjustHeight();

private:
	QRect           m_parentRc;
	int             m_intervel;
	QLabel*         m_nameLabel;
	QLabel*         m_componentLabel;
};
