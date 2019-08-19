module.exports = {
  base: '/',
  themeConfig: {
    repo: 'particle-iot/roadshow-workshop-2019',
    editLinks: true,
    editLinkText: 'Help us improve this page!',
    nav: [
      { text: 'Home', link: '/' },
      { text: 'Team', link: 'http://particle.io' }
    ],
    sidebar: [
      '/docs/',
      ['/docs/ch1', 'Chapter 1: Getting your Particle Argon online'],
      [
        '/docs/ch2',
        'Chapter 2: Working with Particle primitives & Grove Sensors'
      ],
      ['/docs/ch3', 'Chapter 3: Using Particle Webhooks & Integrations'],
      ['/docs/extra', 'Extra: Going Even Further!']
    ]
  },
  title: 'Particle Roadshow Workshop',
  description:
    'Workshops designed to teach the basics of IoT development with the Particle ecosystem & the Particle Argon'
};
