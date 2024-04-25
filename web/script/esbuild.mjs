import * as esbuild from "esbuild";

const PROJECT_DIR = "../";
const OUT_DIR = PROJECT_DIR + "data/usr/share/mbedhttp/html/";

console.log(OUT_DIR);

let ctx = await esbuild.context({
  entryPoints: ["src/network.ts"],
  bundle: true,
  minify: true,
  sourcemap: true,
  loader: { ".htm": "file" },
  target: ["chrome58", "firefox57", "safari11", "edge16"],
  outdir: `${OUT_DIR}/js`,
});

let { host, port } = await ctx.serve({
  servedir: OUT_DIR,
});
await ctx.watch();

console.log(`[serve] listening at http://${host}:${port}`);
