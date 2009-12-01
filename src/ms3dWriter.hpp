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
 * MS3D writer.
 */

#ifndef ___HPP_GPEXPORT_MS3D_WRITER
#define ___HPP_GPEXPORT_MS3D_WRITER

#include <vector>
#include <string>
#include <map>

#include "basic_types.hpp"
#include "fileWriter.hpp"
#include "conversion.hpp"

namespace MS3DTranslation
{
    using namespace GPTranslation;

    class MS3DWriter
	: virtual protected GPBase,
	  protected FileWriter,
	  protected Converter
    {
    public:
	// overwrites the virtual method (but still calls the original one)
	virtual void WriteScene(const Scene& sceneToWrite,
				const std::string& fileName);
	
    protected:
	/**
	 * writes the complete scene.
	 */
	virtual void doWriteScene(const Scene& sceneToWrite);

    private:
	void writeHeader();
	int nbVertices(const Scene& scene);
	void fillJointNameVector(const JointNode& node,
				 std::vector<std::string>* nameVector);
	const std::map<std::string, uint32> getNameIndexMap(const Scene& scene);
	char getBoneIndex(uint32 vertexIndex,
			  const Mesh& mesh,
			  const std::map<std::string, uint32> nameIndexMap); // maps the jointName to the index it will have later on.
	void writeVertices(const Mesh& meshToWrite,
			   const std::map<std::string, uint32> nameIndexMap);
	void writeVertices(const Scene& sceneToWrite);
	unsigned int writeTriangles(const Mesh& meshToWrite, unsigned int indexOffset, byte groupIndex);
	void writeTriangles(const Scene& sceneToWrite);
	void writeGroups(const Scene& sceneToWrite);
	void writeMaterials(const Scene& sceneToWrite);
	void writeRotation(const MMatrix& matrix);
	void writePosition(const MMatrix& matrix);
	void writeKeyFrameRotation(const KeyFrame& keyFrame, const MMatrix& inverse);
	void writeKeyFramePosition(const KeyFrame& keyFrame, const MMatrix& inverse);
	void writeTrack(const Track& track, const JointNode& node, const MTime& start, const MTime& end);
	uint32 countJoints(const JointNode& node);
    	void writeJoints(const JointNode& node,
			 const std::string& parentName,
			 const Scene& scene);
	void writeJoints(const Scene& sceneToWrite);
	void writeAnimation(const Scene& sceneToWrite);
	void writeMeshes(const Scene& sceneToWrite);
    };
} // namespace MS3DTranslation
#endif // include guard
