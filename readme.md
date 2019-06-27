# Unnecesarily native Least Recently Used Cache
 > B-bu-but can't we just implement a linked list in js and use the built in map instead?
 
 
### Use cases
 1. you can use it to support ephemeral sessions that are specific and bounded to your process.
 2. you can use it as cache for serving static assets through your node process.
 3. you want a cache system that works with v8 garbage collector where if the object isnt visible to the environment, and gc collects it, the cache invalidates its record. 

### install and build

### usage
```javascript
const unlru = require("unnecessarily-native-lru");

//create a cache that has upto 1024 slots
let cache = unlru.Factory({reserved: 1024});

//or
cache = new unlru.lru({reserved: 12});

cache.set("some/path/we/want/to/remember", {foo: "bar"});
cache.set("some-other/path/we/want/to/remember", {bar: "foo"});

let obj = cache.get("some/path/we/want/to/remember");
//obj = {foo: "bar"}
obj = null; 

let metadata = cache.meta("some/path/we/want/to/remember");
//Metadata { last: 1, birth: 0, hits: 1 }

metadata instanceof unlru.metadata;
//true

let itdoesntexist = cache.meta("some/path/we/want/to/remember/but/not/in/cache");
//null

//frees it from the cache and removes references to that object. if no other variable references the
//object, it tells v8 that it is okay to remove it.
cache.evict("some/path/we/want/to/remember"); 

cache.get("some/path/we/want/to/remember");
//returns undefined

//this variation weakly evicts the entry, that is, it doesn't remove the entry in cache
//until the v8 GC removes it, and then the cache record is invalidated.
cache.evict("some-other/path/we/want/to/remember", true);

//as long as the GC didn't collect, the cache record is still accessible.
cache.get("some-other/path/we/want/to/remember"); 
//returns {bar: "foo"}

//and there is also
cache.clear();
//which invalidates the whole cache and removes its references to all of the underlying JS objects 
//(if no other refences are held, the GC could eventually collect those objects).

```
