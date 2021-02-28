# Glossary

This pages summarizes different concepts used in Texpainter.

## Image

An image is a matrix of pixels. Its purpose in Texpainter is sort of similar to a "layer" in a more
conventional image processing application like GIMP, Krita, or Adobe® PhotoShop®, but since
Texpainter uses a graph instead of a stack for compositing, they are not really layers.

## Image spectrum

The image spectrum is basically the fourier transfrom of an image. Notice that this has nothing to
do with wavelength in time domain (an image is already a fourier transform with respect to time).
Instead, it is the fourier transform with respect to the spatial coordinates.

## Image processor

An image processor is an entity that is used to process data in the
<a href="app/compositor.html">Compositor</a>. Most often they process images, but they can also
operate on other data.

## Node

An node is an entity within a graph. In texpainter, nodes wrap image processors to direct data flow.

## Palette

A palette in Texpainter consists of 16 colors. The main purpose palettes in texpainter is to help to
create a "consistent" image. This means that you typically select some colors and shades of these,
and use these colors for all drawing operations. Palettes can also be utilized from within the
compositor as a colormap.

## Port
A termination point of a connection. It can either be an input port or an output port, depending on
signal direction.

## Supersampling

Supersampling is a technique used to increase render quality. The basic idea is to render a larger
image, and then to downsample the result. While this technique requires extra system resources it is
easy to implement, and does not add any extra work when implementing image processors.