#include "listwidget.h"

//源文件
//////////////////////////////////////////////////////////////////////////////////////////
///DesignSchemeColorsWidget
DesignSchemeColorsWidget::DesignSchemeColorsWidget(QWidget *parent /*= nullptr*/)
	:QListWidget(parent)
	, m_tipWidget(nullptr)
{
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setIconSize(QSize(_SS(COLOR_ICON_WIDTH, COLOR_ICON_WIDTH)));
	setResizeMode(QListView::Adjust);
	setViewMode(QListView::IconMode);
	setAttribute(Qt::WA_TranslucentBackground);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	horizontalScrollBar()->setEnabled(false);
	verticalScrollBar()->setEnabled(false);
	setWrapping(true);
	setDragEnabled(false);
	setFocusPolicy(Qt::NoFocus);
	//setTopMargin(_S(CARD_LIST_MARGIN_TOP));
	setMovement(QListView::Static);
	setSpacing(_S(1));
	setFixedWidth(_S(LIST_FIX_WIDTH));
	setItemDelegate(new DesignSchemeColorsItemDelegate(this));
	setMouseTracking(true);
	installEventFilter(this);
	if (parent)
		parent->installEventFilter(this);
	connect(this, &QListWidget::itemEntered, this, &DesignSchemeColorsWidget::onItemHover);
}
DesignSchemeColorsWidget::~DesignSchemeColorsWidget()
{
	if (m_tipWidget)
	{
		delete m_tipWidget;
		m_tipWidget = nullptr;
	}
}
void DesignSchemeColorsWidget::setColors(const PaletteColorInfos &vec)
{
	for (int i = 0; i < vec.count(); i++)
	{
		appendItem(vec[i]);
	}
}
void DesignSchemeColorsWidget::onItemHover(QListWidgetItem *widgetItem)
{
	if (!m_tipWidget)
		m_tipWidget = new SchemeItemTipsWidget(nullptr);
	if (!widgetItem)
	{
		m_tipWidget->setVisible(false);
		return;
	}
	PaletteColorInfo *info = (PaletteColorInfo *)widgetItem->data(LIST_ITEM_DATA).toLongLong();
	if (!info)
	{
		m_tipWidget->setVisible(false);
		return;
	}
	m_tipWidget->clear();
	QRect itemRect = visualItemRect(widgetItem);
	QPoint point = mapToGlobal(QPoint(itemRect.topLeft()));
	if (!info->name().isEmpty())
		m_tipWidget->setName(tr("Color Name") + ": " + info->name());
	m_tipWidget->setComponents(QString(tr("Apply Component") + ": " + "upper/bottom"));
	QRect geoRc(point.x(), point.y(), itemRect.width(), itemRect.height());
	m_tipWidget->setParentGrometry(geoRc);
	m_tipWidget->setShowIntervel(_S(2));
	m_tipWidget->show();
}
void DesignSchemeColorsWidget::leaveEvent(QEvent *event)
{
	setCursor(Qt::ArrowCursor);
	onItemHover(nullptr);
	QListWidget::leaveEvent(event);
}
void DesignSchemeColorsWidget::appendItem(const PaletteColorInfo &item)
{
	QListWidgetItem *listWidgetItem = new QListWidgetItem();
	listWidgetItem->setSizeHint(QSize(_SS(COLOR_ICON_WIDTH, COLOR_ICON_WIDTH)));
	PaletteColorInfo *newItem = new PaletteColorInfo(item);
	listWidgetItem->setData(LIST_ITEM_DATA, (qlonglong)newItem);
	listWidgetItem->setData(LIST_ITEM_TYPE, LIST_ITEM_NORMAL_TYPE);
	addItem(listWidgetItem);
	resizeWidget();
}
void DesignSchemeColorsWidget::clearAllItem()
{
	while (count())
	{
		constexpr int delNum = 0;
		QListWidgetItem *widgetItem = item(delNum);
		if (!widgetItem)
			continue;
		if (widgetItem->data(LIST_ITEM_TYPE).toInt() == LIST_ITEM_ADD_TYPE)
		{
			QWidget *itemWidget = this->itemWidget(widgetItem);
			if (itemWidget)
			{
				takeItem(delNum);
				itemWidget->deleteLater();
				delete widgetItem;
			}
		}
		else
		{
			PaletteColorInfo *info = (PaletteColorInfo *)widgetItem->data(LIST_ITEM_DATA).toLongLong();
			if (!info)
			{
				continue;
			}
			takeItem(delNum);
			delete widgetItem;
			delete info;
		}
	}
	clear();
	repaint();
}
void DesignSchemeColorsWidget::resizeWidget()
{
	// STYLE中设置
	QMargins margin = contentsMargins();
	// 算出行，列数
	int wi = width();
	int col = static_cast<int> ((width() - margin.left() - margin.right()) / COLOR_ICON_WIDTH);//列
	if (col <= 0)
		col = 1;
	int itemCount = count();
	int row = static_cast<int> (itemCount / col);//行
	if (itemCount % col > 0)
		row++;
	if (row < 1)
		row = 1;
	int itemH = COLOR_ICON_WIDTH;
	int h = itemH * row + margin.top() + margin.bottom() + _S(2);
	setFixedHeight(h);
}

//////////////////////////////////////////////////////////////////////////////////////
///DesignSchemeColorsItemDelegate
DesignSchemeColorsItemDelegate::DesignSchemeColorsItemDelegate(QObject* pParent)
	:QStyledItemDelegate(pParent)
{
}
DesignSchemeColorsItemDelegate::~DesignSchemeColorsItemDelegate()
{
}
QPixmap DesignSchemeColorsItemDelegate::createGradientColorImg(const QByteArray &json, QSize size) const
{
	if (json.isEmpty())
		return QPixmap();
	QPixmap pixmap(size.width(), size.height());
	pixmap.fill(Qt::transparent);
	QPainter p(&pixmap);
	p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	QLinearGradient linearGradient
		= genericfunc::generateGradientByGradientInfo(JsonUtils::getJsonObjectFromString(json));
	p.setBrush(linearGradient);
	p.setPen(Qt::transparent);
	p.drawRect(0, 0, size.width(), size.height());
	return pixmap;
}

void DesignSchemeColorsItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	int itemType = index.data(LIST_ITEM_TYPE).toInt();
	if (itemType == LIST_ITEM_ADD_TYPE)
		return;
	DesignSchemeColorsWidget *colListWidget = qobject_cast<DesignSchemeColorsWidget *>(option.styleObject);
	if (!colListWidget)
		return;
	QRect contentRect = option.rect; // 目标矩形
	QRect borderRect = option.rect;
	const int contentMargin = _S(3);
	const int borderMargin = _S(1);
	const int radius = 2;
	contentRect.adjust(contentMargin, contentMargin, -contentMargin, -contentMargin);
	borderRect.adjust(borderMargin, borderMargin, -borderMargin, -borderMargin);
	// 取得该项对应的数据
	PaletteColorInfo *info = (PaletteColorInfo *)index.data(LIST_ITEM_DATA).toLongLong();
	painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	painter->save();
	painter->setBrush(Qt::transparent);
	QPen pen;
	pen.setWidthF(_S(1.5));
	pen.setColor(Qt::transparent);
	if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver)
		pen.setColor(QColor(ColorDef::clr_primary_c));
	else
		pen.setColor(Qt::transparent);
	painter->setPen(pen);
	painter->drawRoundedRect(borderRect, _S(radius), _S(radius));
	painter->restore();
	painter->setPen(Qt::transparent);
	if (info->colorType() == EmColorItemType::RGB)
	{
		QColor color;
		color.setRed(info->R());
		color.setGreen(info->G());
		color.setBlue(info->B());
		painter->setBrush(color);
		painter->drawRoundedRect(contentRect, _S(radius), _S(radius));
	}
	else
	{
		QPixmap img = createGradientColorImg(info->gradientInfo().toUtf8(), QSize(borderRect.width(), borderRect.height()));
		painter->setBrush(img);
		painter->drawRoundedRect(contentRect, _S(radius), _S(radius));
	}
}
