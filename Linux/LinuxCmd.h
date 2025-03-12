/*
 * @Description: Linux指令控制
 * @Author: Ivy
 * @Date: 2025-03-12 09:34:43
 * @LastEditTime: 2025-03-12 09:34:43
 * @LastEditors: Ivy
 */

#pragma once

void exeCmd(const QString sCmd);

void setGpioCtrl(QString sGpio, int ctrl_status);
int getGpioCtrl(QString sGpio);

