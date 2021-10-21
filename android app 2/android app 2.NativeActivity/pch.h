﻿//
// pch.h
// Заголовок стандартных системных включаемых файлов.
//
// Используется системой сборки для создания предкомпилированного заголовка. Обратите внимание, что
// pch.cpp не нужен, а pch.h автоматически включается в состав всех файлов cpp,
// которые являются частью проекта
//

#include <jni.h>
#include <errno.h>

#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <vector>

#include <android/sensor.h>

#include <android/log.h>
#include "android_native_app_glue.h"

#include "vec2/vec2.h"
#include "color/color.h"

#include "drawing/drawing.h"
#include "game/game.h"
