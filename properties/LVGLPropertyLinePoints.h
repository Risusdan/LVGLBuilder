#ifndef LVGLPROPERTYLINEPOINTS_H
#define LVGLPROPERTYLINEPOINTS_H

/**
 * @file LVGLPropertyLinePoints.h
 * @brief LVGLProperty subclass for lv_line point array.
 */

#include "LVGLPropertyPoints.h"
#include "LVGLObject.h"

class LVGLPropertyLinePoints : public LVGLPropertyPoints
{
public:
	QString name() const { return "Points"; }

	QStringList function(LVGLObject *obj) const {
		lv_line_ext_t * ext = reinterpret_cast<lv_line_ext_t*>(lv_obj_get_ext_attr(obj->obj()));
		QStringList ret;
		QString var = obj->name() + "_points";
		ret << "static lv_point_t " + var + "[" + QString::number(ext->point_num) + "] = {";
		for (uint16_t i = 0; i < ext->point_num; ++i)
			ret << "\t{" + QString::number(ext->point_array[i].x) + "," + QString::number(ext->point_array[i].y) + "},";
		ret << "\t};";
		ret << QString("lv_line_set_points(%1, %2, %3);").arg(obj->codeName()).arg(var).arg(ext->point_num);
		return ret;
	}

protected:
	QVector<lv_point_t> get(LVGLObject *obj) const {
		lv_line_ext_t * ext = reinterpret_cast<lv_line_ext_t*>(lv_obj_get_ext_attr(obj->obj()));
		QVector<lv_point_t> ret(ext->point_num);
		memcpy(ret.data(), ext->point_array, ext->point_num * sizeof(lv_point_t));
		return ret;
	}
	void set(LVGLObject *obj, QVector<lv_point_t> data) {
		lv_line_ext_t * ext = reinterpret_cast<lv_line_ext_t*>(lv_obj_get_ext_attr(obj->obj()));
		if (ext->point_num && ext->point_array)
			delete [] ext->point_array;
		lv_point_t *points = new lv_point_t[static_cast<size_t>(data.size())];
		memcpy(points, data.data(), static_cast<size_t>(data.size()) * sizeof(lv_point_t));
		lv_line_set_points(obj->obj(), points, static_cast<uint16_t>(data.size()));
	}
};

#endif // LVGLPROPERTYLINEPOINTS_H
