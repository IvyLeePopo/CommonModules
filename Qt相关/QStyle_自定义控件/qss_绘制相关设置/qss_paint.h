/*
 * @Description: qss���߻����������
 * @Author: Ivy
 * @Date: 2022-05-06 09:34:43
 * @LastEditTime: 2022-05-06 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

//1��QWidgetʹ��setStyleSheet������ʽ����Ч
void qssNotWork();

//2����paintEvent�л���ͼ��
void paintIconInPaintEvent();

//3������������QSS������qss�ļ�·��
void setWidgetQss(QWidget* widget, const QString& strQssPath)��

//4�����Ʊ߿���������
void paintBoder();

//5��qt���֣����²���
void reLayout();