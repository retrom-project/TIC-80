// SPDX-License-Identifier: MIT
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {pathToFileURL} from 'node:url';
const file=process.argv[2];
const create=(await import(pathToFileURL(file))).default;
const code=new TextEncoder().encode(`function BOOT() x=pmem(0) end\nfunction TIC() if btn(3) then x=x+1 end if btnp(4) then pmem(0,x) end cls(0) rect(x,20,5,5,8) end\n`);
const cart=new Uint8Array(8+code.length);cart.set([17,0,0,0,5,code.length&255,code.length>>8,0]);cart.set(code,8);
async function instance(){
 const m=await create({wasmBinary:await readFile(file.replace(/mjs$/,'wasm'))});
 assert.equal(m._retrom_abi(),1);
 const p=m._malloc(cart.length);m.HEAPU8.set(cart,p);assert.equal(m._retrom_load(p,cart.length),1);m._free(p);return m;
}
const a=await instance();for(let i=0;i<20;i++)assert.equal(a._retrom_step(8),1);
assert.equal(a._retrom_step(16),1);
const p=a._retrom_state();const state=a.HEAPU8.slice(p,p+1024);
assert.equal(new DataView(state.buffer).getUint32(0,true),20);
const b=await instance();const q=b._malloc(1024);b.HEAPU8.set(state,q);
assert.equal(b._retrom_restore(q,1023),0);assert.equal(b._retrom_restore(q,1024),1);
assert.equal(b._retrom_step(0),1);
function pixels(m){const p=m._retrom_pixels();return m.HEAPU8.slice(p,p+240*136*4);}
assert.deepEqual(pixels(a),pixels(b));
assert.equal(b._retrom_step(8),1);assert.notDeepEqual(pixels(a),pixels(b));
assert.equal(b._retrom_restore(q,1024),0);b._free(q);a._retrom_stop();b._retrom_stop();
console.log('TIC-80 native pmem, preboot restore, resumed input and bounds PASS');
