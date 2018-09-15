#include <json/json.h>
#include <stdio.h>

int main() {
  /*Creating a json object*/
  json_object * jobj = json_object_new_object();

  /*Creating a json array*/
  json_object *jarray = json_object_new_array();

  /*Creating json strings*/
  json_object *jstring1 = json_object_new_string("c");
  json_object *jstring2 = json_object_new_string("c++");
  json_object *jstring3 = json_object_new_string("php");

  /*Adding the above created json strings to the array*/
  json_object_array_add(jarray,jstring1);
  json_object_array_add(jarray,jstring2);
  json_object_array_add(jarray,jstring3);
  printf ("The json object created: %s\n",json_object_to_json_string(jarray));
  jarray = json_object_new_array();
  printf ("The json object created: %s\n",json_object_to_json_string(jarray));
  /*Form the json object*/
  json_object_object_add(jobj,"Categories", jarray);
  json_object_object_add(jobj,"Ca", jarray);
  json_object_object_add(jobj,"Caries", jarray);

  /*Now printing the json object*/
  printf ("The json object created: %s\n",json_object_to_json_string(jobj));

}
