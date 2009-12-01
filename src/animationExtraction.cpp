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
 * Extracts the skeleton-keyframes (=Animation) out of Maya.
 * this is definitely not complete (and I already found some scenes, where
 * the extraction is not working).
 * Maya provides a more complete example in
 *         devkit/plug-ins/animImportExport.cpp
 */

#include "animationExtraction.hpp"

#include <maya/MDagPath.h>
#include <maya/MAnimUtil.h>
#include <maya/MItDag.h>
#include <maya/MTime.h>
#include <maya/MFnTransform.h>
#include <maya/MAnimControl.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MPlugArray.h>
#include <maya/MObjectArray.h>

#include <vector>
#include <set>

namespace GPTranslation
{
    // runs over all animated joints, and extracts the keyframe-times:
    // finds the animated plugs, and gets all curve-times.
    std::set<MTime>
    AnimationExtractor::extractKeyFrameTimes(const MDagPath& jointPath) const
    {
	std::set<MTime> jointTimes;
	
	// extract the key-frame times
	MPlugArray animatedPlugs;
	MAnimUtil::findAnimatedPlugs(jointPath, animatedPlugs);
	for (unsigned int i = 0; i < animatedPlugs.length(); ++i)
	{
	    MObjectArray curves;
	    MAnimUtil::findAnimation(animatedPlugs[i], curves);
	    for (unsigned int j = 0; j < curves.length(); ++j)
	    {
		MFnAnimCurve curve(curves[j]);
		for (unsigned int k = 0; k < curve.numKeys(); ++k)
		    jointTimes.insert(curve.time(k));
	    } // for curves
	} // for animatedPlugs
	
	return jointTimes;
    }

    // finds all animated joints.
    std::vector<MDagPath>
    AnimationExtractor::getAnimatedJoints() const
    {
	std::vector<MDagPath> jointPaths;
        // get all bones, and their keyframe-times
        for (MItDag dagIt(MItDag::kDepthFirst, MFn::kJoint, &status);
             !dagIt.isDone();
             dagIt.next())
        {
            MDagPath jointPath;
            dagIt.getPath(jointPath);

            if (!MAnimUtil::isAnimated(jointPath))
            {
                cout << "not animated" << endl;
                continue;
            }

            jointPaths.push_back(jointPath);
	}
	return jointPaths;
    }


    // for every animated joint jointI and each of its times puts a
    // pair [indexTime, jointI] into a map and returns it.
    Times2JointMap
    AnimationExtractor::getTimes2JointMap(
    	const std::vector<MDagPath>& animatedJointPaths) const
    {
	Times2JointMap times2JointMap;
	
	for (uint32 i = 0; i < animatedJointPaths.size(); ++i)
	{
	    MDagPath jointPath = animatedJointPaths[i];
	    
	    std::set<MTime> jointTimes = extractKeyFrameTimes(jointPath);
	    
	    // the jointTimes-set contains all key-times this joint is 
	    // animated with.
            for (std::set<MTime>::const_iterator it = jointTimes.begin();
                 it != jointTimes.end();
                 ++it)
                // insert pairs [time, {animatedJointPath}Index] into
                // the times2JointMap.
                times2JointMap.insert(Times2JointMap::value_type(*it, i)); 
        }

	return times2JointMap;
    }

    uint32
    AnimationExtractor::getFps() const
    {
        switch(MTime::uiUnit())
        {
            case MTime::kSeconds:       return 1;
            case MTime::kMilliseconds:  return 1000;
            case MTime::kGames:         return 15;
            case MTime::kFilm:          return 24;
            case MTime::kPALFrame:      return 25;
            case MTime::kNTSCFrame:     return 30;
            case MTime::kShowScan:      return 48;
            case MTime::kPALField:      return 50;
            case MTime::kNTSCField:     return 60;
            default:                    return 1;
            // We are ignoring the following time-units.
            //case MTime::kHours:       return 1;
            //case MTime::kMinutes:     return 1;
        }
    }
    
    // be warned: we are only extracting bone-animation-data here.
    //  furthermore the animation-extraction is independent of the
    //  skeleton-extraction.
    Animation
    AnimationExtractor::ExtractAnimation() const
    {
        cout << std::endl << " Extracting Animation ";

        Animation result;
        result.fps = getFps();

	
	std::vector<MDagPath> animatedJointPaths = getAnimatedJoints();
	
	if (animatedJointPaths.size() == 0) return result;

	Times2JointMap times2JointMap = getTimes2JointMap(animatedJointPaths);

	if (times2JointMap.size() == 0) return result;

	result.start = times2JointMap.begin()->first; // start-time
	result.end = times2JointMap.rbegin()->first; // end-time

	// this is essentially an optimization: instead of calling
	// partialPathName all the time, we store the name in a seperate
	// array.
	std::string jointNames[animatedJointPaths.size()];
	for (uint32 i = 0; i < animatedJointPaths.size(); ++i)
	    jointNames[i] = animatedJointPaths[i].partialPathName().asChar();

	uint32 nbKeyTimes = 0; // just for logging

        // save initial time to go back after we have extracted the
        // transformations.
        const MTime initialTime = MAnimControl::currentTime();
        
        // mayaTime = time Maya is set to. (property stays true in the
        // following loop). I avoided naming it "currentTime", as i name
        // my loop-vars currentXX.
        MTime mayaTime = initialTime;

        // times2Joint is a multiset and hence sorted.
        for (Times2JointMap::const_iterator it = times2JointMap.begin();
             it != times2JointMap.end();
             ++it)
        {
            if (mayaTime != it->first)
            {
                nbKeyTimes++; // just for logging
                MAnimControl::setCurrentTime(it->first);
                mayaTime = it->first;
            }
            Track&   currentTrack     = result.tracks[jointNames[it->second]];
            MDagPath currentJointPath = animatedJointPaths[it->second];
            KeyFrame newKeyFrame;
            newKeyFrame.time = it->first;
            // extract the transformation for this joint.
            MFnTransform transform(currentJointPath);
            newKeyFrame.transformation = transform.transformation().asMatrix();
            currentTrack.push_back(newKeyFrame);
        }
        // back to initial time
        MAnimControl::setCurrentTime(initialTime);

        cout << endl;
        cout << "  extracted " << result.tracks.size() << " tracks" << endl;;
        cout << "  with " << nbKeyTimes
             << " key-times (might be incorrect: could be +1.)" << endl;
        return result;
    }

} // namespace GPTranslation
