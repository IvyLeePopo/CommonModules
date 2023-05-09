/*
 * @Description: QListWidget��ʽ��ʼ������һ�����ӣ���ɫ�����б�
 * @Author: Ivy
 * @Date: 2022-04-25 09:34:43
 * @LastEditTime: 2023-02-25 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once
 ///ͷ�ļ�

 ///��ɫ����-����
class DesignSchemeColorsItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	DesignSchemeColorsItemDelegate(QObject* pParent);
	virtual ~DesignSchemeColorsItemDelegate();
private:
	QPixmap createGradientColorImg(const QByteArray &json, QSize size)const;
private:
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


class DesignSchemeColorsWidget : public QListWidget
{
	Q_OBJECT
public:
	DesignSchemeColorsWidget(QWidget *parent = nullptr);
	~DesignSchemeColorsWidget();
	void setColors(const PaletteColorInfos &vec);//PaletteColorInfos���ݵĽṹ��
public slots:
	void onItemHover(QListWidgetItem *item);
protected:
	virtual void leaveEvent(QEvent *event) override;
private:
	void appendItem(const PaletteColorInfo &item);
	void clearAllItem();
	void resizeWidget();
private:
	SchemeItemTipsWidget                *m_tipWidget;//������ʾ
};

