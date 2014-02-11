#include <gst/gst.h>
#include <string.h>

void gst_play_uri(const char* uri) {
    GstElement* pipeline;
    GstBus* bus;
    GstMessage* msg;

    char str[256] = "playbin2 uri=";
    strncat(str, uri, 256);
    pipeline = gst_parse_launch(str, NULL);
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    if(msg != NULL)
        gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}

int main(int argc, char** argv) {
    if(argc != 2)
        return 1;
    gst_init(NULL, NULL);
    gst_play_uri(argv[1]);
    return 0;
}
