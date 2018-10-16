## build

## usage
```javascript
//create a cache that has upto 1024 slots. 
const unlru = require("unnecessarily-native-lru").Factory({reserved:1024});

unlru.set("some/path/we/want/to/remember", {foo: "bar"});
unlru.set("some-other/path/we/want/to/remember", {bar: "foo"});

let obj = unlru.get("some/path/we/want/to/remember");

let metadata = unlru.meta("some/path/we/want/to/remember");
//metadata = {hits: 2, birth: 0, lastaccessed: 2};

//frees it from the cache and removes references to that object. if no other variable references the
//object, it tells v8 to remove the handle.
unlru.evict("some/path/we/want/to/remember"); 

//this variation weakly evicts the entry, that is it doesn't remove the entry in cache
//until the v8 GC removes it, and then the cache record is invalidated.
unlru.evict("some-other/path/we/want/to/remember", true);

//as long as the GC didn't collect, the cache record is still accessible.
unlru.get("some-other/path/we/want/to/remember"); 
//returns {bar: "foo"}


```
