platforms = {
  iOS = {
    screens = {
      [""] = { w = 480, h = 320, ppi=163, scale=0.25 },
      [".hd"] = { w = 960, h = 640, ppi=326, scale=0.5 },
      [".hdx"] = { w = 2048, h = 1536, ppi=326, scale=1 },
    },
    fake_links = false,
  },
  Android = {
    screens = {
      [""] = { w = 480, h = 320, ppi=163, scale=0.25 },
      [".hd"] = { w = 960, h = 640, ppi=326, scale=0.5 },
      [".hdx"] = { w = 2048, h = 1536, ppi=326, scale=1 },
    },
    fake_links = true,
  },
  X11 = {
    screens = { [""] = { w = 1920, h = 1080, ppi=128, scale=1 } },
    fake_links = false,
  },
  OSX = {
    screens = { [""] = { w = 1920, h = 1080, ppi=128, scale=1 } },
    fake_links = false,
  },
}


