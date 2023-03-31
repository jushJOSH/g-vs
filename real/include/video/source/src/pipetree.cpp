#include <video/source/pipetree.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>

using boost::format;
using boost::str;

PipeTree::PipeTree() 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    pipeline(gst_pipeline_new(uuid.c_str())),
    decoder(gst_element_factory_make("decodebin", str(format("%1%_decodebin") % uuid).c_str())),
    converter(gst_element_factory_make("videoconvert", str(format("%1%_videoconvert") % uuid).c_str())),
    tee(gst_element_factory_make("tee", str(format("%1%_tee") % uuid).c_str()))
{
    g_print("Created tree %s", uuid.c_str());
    
    // Add to pipeline
    gst_bin_add_many(GST_BIN(pipeline), decoder, converter, tee, NULL);
    bool isLinked = gst_element_link_many(decoder, converter, tee, NULL);

    // If linked failed throw something funny
    if (!isLinked) 
        throw std::runtime_error("Error creating PipeTree object: Link failed!");
}

PipeTree::PipeTree(const std::string& source)
:   PipeTree()
{
    if (!setSource(source)) 
        throw std::runtime_error("Error creating PipeTree object: Link failed on setting source");
}

PipeTree::~PipeTree() {
    g_print("Deleted tree %s", uuid.c_str());

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}

bool PipeTree::addBranch(const std::string &name, std::shared_ptr<PipeBranch> branch) {
    branch->loadBin(GST_BIN(pipeline));
    
    bool isLinked = gst_element_link(tee, branch->getQueue());
    if (isLinked) branches[name] = branch;

    return isLinked;
}

void PipeTree::removeBranch(const std::string& name) {
    if (branches.contains(name)) branches.erase(name);
}

GstElement* PipeTree::getSink(const std::string &name) {
    return branches.contains(name) 
        ? branches[name]->getSink() 
        : nullptr;
}

bool PipeTree::setSource(const std::string &source) {
    this->source = gst_element_factory_make(source.c_str(), str(format("%1%_source") % uuid).c_str());
    return gst_element_link(this->source, decoder);
}

GstStateChangeReturn PipeTree::setState(GstState state) {
    return gst_element_set_state(pipeline, state);
}