/**
 * @file LVGLProject.h
 * @brief Project metadata and persistence — save/load JSON, export C code.
 */

#ifndef LVGLPROJECT_H
#define LVGLPROJECT_H

#include <QSize>
#include <QString>

/**
 * @class LVGLProject
 * @brief Holds project metadata and implements file I/O.
 *
 * A project encapsulates a name and display resolution. Persistence is handled
 * via a JSON format (version 1) that includes the widget tree, custom styles,
 * image assets, and font assets. C code export generates a .h/.c pair with
 * lv_xxx_create() calls and style setup.
 *
 * The JSON file format:
 * @code
 * {
 *   "version": 1,
 *   "lvgl": { "name": "...", "resolution": {...}, "widgets": [...] },
 *   "images": [...],
 *   "fonts": [...]
 * }
 * @endcode
 */
class LVGLProject {
 public:
  /** @brief Default constructor — creates "App" at max LVGL resolution. */
  LVGLProject();
  /** @brief Constructs a project with the given name and resolution. */
  LVGLProject(const QString &name, QSize resolution);

  /** @brief Returns the project name. */
  QString name() const;
  /** @brief Returns the display resolution. */
  QSize resolution() const;

  /**
   * @brief Loads a project from a JSON file.
   * @param fileName Path to the .json file.
   * @return The loaded project, or nullptr on failure (invalid format, unsupported version).
   *
   * Restores images, fonts, screen color, and the full widget tree.
   * Shows an error dialog if an image file cannot be loaded.
   */
  static LVGLProject *load(const QString &fileName);

  /**
   * @brief Saves the current project state to a JSON file.
   * @param fileName Destination path.
   * @return true on success.
   *
   * Serializes all top-level objects (which recursively include children),
   * all images with file paths, and all custom fonts.
   */
  bool save(const QString &fileName);

  /**
   * @brief Exports the project as C source code.
   * @param path Output directory path.
   * @return true on success.
   *
   * Generates a .h file with extern declarations and a .c file with
   * a create function that reconstructs the UI. Also saves image and
   * font assets as separate .c files.
   */
  bool exportCode(const QString &path) const;

  /** @brief Returns the file path used in the last save/load operation. */
  QString fileName() const;
  /** @brief Sets the project name. */
  void setName(const QString &name) { m_name = name; }

 private:
  QString m_name;
  QString m_fileName;
  QSize m_resolution;
};

#endif  // LVGLPROJECT_H
