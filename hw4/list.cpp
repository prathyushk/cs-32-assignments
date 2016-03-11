void listAll(const Class* c, string path)
{
  if(c == nullptr)
    return;
  cout << path << c->name() << endl;
  path = path + c->name() + "=>";
  for(int i = 0; i < c->subclasses().size(); i++)
    listAll(c->subclasses()[i],path);
}
