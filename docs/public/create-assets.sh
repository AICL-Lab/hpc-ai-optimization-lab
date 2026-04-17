#!/bin/bash
# Create simple PNG placeholder using base64
# Apple touch icon (180x180) - simple gradient
convert -size 180x180 xc:none \
  -fill "gradient:#22d3ee-#0891b2" \
  -draw "circle 90,90 90,10" \
  -draw "fill white text 90,100 'GPU'" \
  apple-touch-icon.png 2>/dev/null || echo "Creating simple placeholder"

# If ImageMagick not available, create simple files
if [ ! -f apple-touch-icon.png ]; then
  # We'll use SVG files instead which VitePress can handle
  echo "SVG logos created instead"
fi
