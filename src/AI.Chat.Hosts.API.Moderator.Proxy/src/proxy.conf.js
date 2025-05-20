const PROXY_CONFIG = [
  {
    context: [
      "/commands",
    ],
    target: 'http://localhost:5118',
    secure: false
  }
]

module.exports = PROXY_CONFIG;
