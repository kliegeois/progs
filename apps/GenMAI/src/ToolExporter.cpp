//   Copyright 2017 Romain Boman
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

#include "Global.h"
#include "ToolExporter.h"

/**
 * @brief Constructor
 */

ToolExporter::ToolExporter(Tool &_matrix): Exporter(), matrix(_matrix)
{
    setBaseFileName( std::string("mymatrix") );
}

void
ToolExporter::writeBody()
{
    writePoints();
    writeCurves();
    writeContours();
}

/**
 * @brief Writes the points to the file
 */

void 
ToolExporter::writePoints() 
{
}

/**
 * @brief Writes the curves to the file
 */

void 
ToolExporter::writeCurves() 
{
}

/**
 * @brief Writes the contours to the file
 */

void 
ToolExporter::writeContours() 
{
}

