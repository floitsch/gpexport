/*
Copyright (c) 2004 Florian Loitsch

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 * optimizes the scene (merge duplicated vertices...)
 */

#ifndef ___HPP_GPEXPORT_OPTIMIZATION
#define ___HPP_GPEXPORT_OPTIMIZATION

#include <maya/MDagPath.h>

#include "gpBase.hpp"
#include "scene.hpp"
#include "mesh.hpp"

namespace GPTranslation
{
    class Optimizer
	: protected virtual GPBase
    {
    public:
	/**
	 * optimizes the scene (removes duplicates...)
	 */
	virtual void OptimizeScene(Scene* sceneToOptimize) const;

    private:
	void optimizeMesh(Mesh* meshToOptimize) const;
    };
} // namespace GPTranslation
#endif // include guard
