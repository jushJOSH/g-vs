#include <video/source/source.hpp>

#include <atomic>

GstFlowReturn Source::on_new_sample(GstElement* appsink, gpointer data)  {
    GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));

    // Setting new sample to selected uuid
    auto arg = (Source::CallbackArg*)data;

    std::lock_guard<std::mutex> locker(arg->mutex);
    arg->samples->push_back(std::make_shared<Sample>(sample));

    return GST_FLOW_OK;
}

Source::Source() 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    arg(std::make_shared<CallbackArg>())
{   
    g_print("Created source %s\n", uuid.c_str());
    arg->samples = std::make_shared<boost::circular_buffer<std::shared_ptr<Sample>>>(10);
}

Source::Source(const std::string &source)
:   Source()
{
    sourceElements = std::make_shared<PipeTree>(source);
}

GstStateChangeReturn Source::setState(GstState state) {
    return sourceElements->setState(state);
}

Source::~Source() {
    g_print("Deleted source %s\n", uuid.c_str());
}

std::shared_ptr<Sample> Source::getSample() {
    std::lock_guard<std::mutex> locker(arg->mutex);

    auto copy = arg->samples->front();
    arg->samples->pop_front();

    return copy;
}

void Source::waitSample() const {
    while (!arg->samples->size());
}

bool Source::addBranch(const std::string &name, std::shared_ptr<PipeBranch> branch) {
    return sourceElements->addBranch(name, branch);
}