# File formats

Texpainter uses json and OpenEXR for data entries. These entries are stored in a wad64 archive, with
the following contents:

| File          |Content                                                                           |
|---------------|----------------------------------------------------------------------------------|
|`document.json`| Contains information about the current workspace, and how data resources are used|
|`data/*`       | Data entries such as images and palettes                                         |

## Image files

Image files are stored using the OpenEXR file format. This is also the format used when importing
and exporting image data. At the time of writing, no other file format is supported. However, it
is possible to convert to and from OpenEXR by using other software, such as ImageMagick.

## Palette files

Palettes are stored in a specific JSON format. The root element has two keys, `colorspace` and
`colors`. `colorspace` can either be `linear` or `g22`. `linear` means that the intensity has linear
encoding, and `g22` that colors are encoded with gamma set to 2.2. `colors` is simply an array of
RGBA values stored as strings. Below is an example that is a representation of PC RGBI color
palette.

```json
{
	"colors": [
		"0.00000000e+00 0.00000000e+00 0.00000000e+00 1.00000000e+00",
		"8.91935069e-02 8.91935069e-02 8.91935069e-02 1.00000000e+00",
		"0.00000000e+00 0.00000000e+00 4.09825738e-01 1.00000000e+00",
		"8.91935069e-02 8.91935069e-02 1.00000000e+00 1.00000000e+00",
		"0.00000000e+00 4.09825738e-01 0.00000000e+00 1.00000000e+00",
		"8.91935069e-02 1.00000000e+00 8.91935069e-02 1.00000000e+00",
		"0.00000000e+00 4.09825738e-01 4.09825738e-01 1.00000000e+00",
		"8.91935069e-02 1.00000000e+00 1.00000000e+00 1.00000000e+00",
		"4.09825738e-01 0.00000000e+00 0.00000000e+00 1.00000000e+00",
		"1.00000000e+00 8.91935069e-02 8.91935069e-02 1.00000000e+00",
		"4.09825738e-01 0.00000000e+00 4.09825738e-01 1.00000000e+00",
		"1.00000000e+00 8.91935069e-02 1.00000000e+00 1.00000000e+00",
		"4.09825738e-01 8.91935069e-02 0.00000000e+00 1.00000000e+00",
		"1.00000000e+00 1.00000000e+00 8.91935069e-02 1.00000000e+00",
		"4.09825738e-01 4.09825738e-01 4.09825738e-01 1.00000000e+00",
		"1.00000000e+00 1.00000000e+00 1.00000000e+00 1.00000000e+00"
	],
	"colorspace": "linear"
}
```