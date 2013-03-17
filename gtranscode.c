#include <gst/gst.h>
#include <string.h>
  
int main (int argc, char *argv[])
{
  GstElement *bin, *filesrc, *decoder, *encoder, *filesink;
  gchar *artist, *title, *ext, *filename;

  /* initialize GStreamer */
  gst_init (&argc, &argv);
  /* check that the argument is there */
  if (argc != 2) {
    g_print ("usage: %s <mp4 file>\n", argv[0]);
    return 1;
  }
  artist = strrchr (argv[1], '/');
  if (artist == NULL)
    artist = argv[1];
  artist = g_strdup (artist);
  ext = strrchr (artist, '.');
  if (ext)
    *ext = '\0';
  title = strstr (artist, " - ");
  if (title == NULL) {
   return 1;
  }
  *title = '\0';
  title += 3;
  bin = gst_pipeline_new ("pipeline");
  g_assert (bin);

  filesrc = gst_element_factory_make ("filesrc", "disk_source");
  g_assert (filesrc);

  decoder = gst_element_factory_make ("h264dec", "decode");
  if (!decoder) {
    return 1;
  }

  /* create the encoder */
  encoder = gst_element_factory_make ("theoraenc", "encoder");
  if (!encoder) {
    g_print ("cound not find plugin \"theoraenc\"");
	return 1;
  }

  filesink = gst_element_factory_make ("filesink", "filesink");
  g_assert (filesink);
  filename = g_strdup_printf ("%s.ogg", argv[1]);       /* easy solution */
  g_object_set (G_OBJECT (filesrc), "location", argv[1], NULL);
  g_object_set (G_OBJECT (filesink), "location", filename, NULL);
  g_free (filename);

  gst_tag_setter_set_merge_mode (GST_TAG_SETTER (encoder), GST_TAG_MERGE_KEEP);
  gst_tag_setter_add (GST_TAG_SETTER (encoder), GST_TAG_MERGE_REPLACE,
      GST_TAG_ARTIST, artist, GST_TAG_TITLE, title, NULL);
  gst_bin_add_many (GST_BIN (bin), filesrc, decoder, encoder, filesink, NULL);

  /* link the elements */
  gst_element_link_many (filesrc, decoder, encoder, filesink, NULL);
	
  /* start playing */
  gst_element_set_state (bin, GST_STATE_PLAYING);

  while (gst_bin_iterate (GST_BIN (bin)));

  /* stop the bin */
  gst_element_set_state (bin, GST_STATE_NULL);

  return 0;
}
