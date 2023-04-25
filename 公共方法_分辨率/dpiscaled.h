/*
 * @Description: 分辨率
 * @Author: Ivy
 * @Date: 2022-04-25 09:34:43
 * @LastEditTime: 2023-02-25 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once
#include "global.h"
#define _S(x) static_cast<int>(global::dpiScaled(x))
#define _SS(w, h) QSize(static_cast<int>(_S(w)), static_cast<int>(_S(h)))
#define _SSS(size) QSize(_S(size.width()), _S(size.height()))
#define _SSF(w, h) QSizeF(_S(w), _S(h))
#define _SR(x, y, w, h) QRect(_S(x), _S(y), _S(w), _S(h))
#define _SRF(x, y, w, h) QRectF(_S(x), _S(y), _S(w), _S(h))
#define _SM(l, t, r, b) QMargins(_S(l), _S(t), _S(r), _S(b))
#define _SMM(margins) QMargins(_S(margins.left()), _S(margins.top()), _S(margins.right()), _S(margins.bottom()))
#define _DS(x) global::dpiDeScaled(x)

