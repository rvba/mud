tri = stone.new("a")

a = tri:add_vertex(0,0,0)
b = tri:add_vertex(1,0,0)
c = tri:add_vertex(1,1,0)

tri:add_face(a,b,c)
tri:build_object()

quad = stone.new("b")

d = 2.1

a = quad:add_vertex(0+d,0,0)
b = quad:add_vertex(1+d,0,0)
c = quad:add_vertex(1+d,1,0)
d = quad:add_vertex(0+d,1,0)

quad:add_face(a,b,c,d)
quad:build_object()

