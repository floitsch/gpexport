/*
 * 2004/06/27
 * Florian Loitsch
 *
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
#include "iExporterConfig.hpp"

namespace MS3DTranslation
{
    using namespace GPTranslation;

    class MS3DWriter
	: virtual protected GPBase,
	  protected FileWriter,
	  protected Converter
    {
	public:
		MS3DWriter(const IExporterConfig *config)
			:m_config(config)
		{
		}
    public:
	// overwrites the virtual method (but still calls the original one)
	virtual void WriteScene(const Scene& sceneToWrite);
	
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
	unsigned int writeTriangles(const Mesh& meshToWrite, unsigned int indexOffset, GPTranslation::byte groupIndex);
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

	const IExporterConfig *m_config;
    };
} // namespace MS3DTranslation
#endif // include guard
