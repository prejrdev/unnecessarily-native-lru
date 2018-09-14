```javascript
const unlru = require("unnecessarily-native-lru").Factory({reserved:1024});

unlru.set("some/path/we/want/to/remember", {foo: "bar"});

let obj = unlru.get("some/path/we/want/to/remember");

let metadata = unlru.meta("some/path/we/want/to/remember");
//metadata = {hits: 2, birth: 0, lastaccessed: 2};

//frees it from the cache. if there is no other references, the GC in node will eventually free it
unlru.evict("some/path/we/want/to/remember"); 


```
